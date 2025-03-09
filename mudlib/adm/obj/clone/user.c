//  -*- LPC -*-
// user.c:  an interactive object representing the player's body intended
//          to interface with a shell to do command processing
//
// Copyright (C) 1996 Tim McIntosh (tmcintos@dm.imaginary.com)
//
// This program is part of the OpenLib Mudlib distribution; it
// is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published
// by the Free Software Foundation; either version 2 of the License,
// or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// If you acquired this program as part of the OpenLib Mudlib
// distribution, you should have received a copy of the GNU
// General Public License in the file /doc/GPL; if not,
// write to the Free Software Foundation, Inc., 675 Mass Ave,
// Cambridge, MA 02139, USA.
//
// Originally written by Tim@Dysfunctional Mud 2/16/96
//
// 22 Apr 96  Tim:  added receive_snoop()
//  7 Jul 96  Tim:  added message blocking and buffering
// 24 Aug 96  Tim:  changed to use terminal_colour()
// 25 Aug 96  Tim:  added window_size() apply to dynamically change screen size
//

#include <mudlib.h>
#include <dirs.h>
#include <login.h>
#include <object_types.h>
#include <daemons.h>
#include <shells.h>

#define MAX_MSGBUF_SIZE 100
#define MSG_ED "ed"
#define MSG_PROMPT "prompt"
#define MSG_SYS "system"

inherit INHERIT_DIR "/more";
inherit LIVING;

private static object connection;   // user should always have a conn.
private static string cap_name;     // name with proper capitalization

private static int blocking;        // 1 if blocking messages 0 if not.
private static string* msgbuf;      // message buffer for blocking & linkdead

private mapping env_vars;           // environment variable mapping
private mapping aliases;            // alias mapping
private string* channels;           // list of subscribed channels

/*
 * Applies
 */
void   initialize();
nomask void remove();
void   window_size(int width, int height);
void   write_prompt();
void   write_ed_prompt(int mode);
void   receive_message(string msgclass, string msg);
void   receive_snoop(string msg);
nomask void net_dead();
nomask int  move_or_destruct(object dest);

/*
 * Restricted Public Functions
 */
nomask void init_player(string username);// called from logind sets name etc.
nomask void reconnect();                 // also called from login daemon
nomask int  set_cap_name(string capname);
nomask void set_channels(string* chans);
nomask void set_blocking(int is_blocking);
nomask int  set_connection(object ob);
nomask int  force_me(string cmd);
nomask int start_ed(string file, int rst);  // input handled by process_input()
nomask void process_ed_input(string input);
int set_cwd(string cwd);
int set_env(string var, mixed val);
nomask mapping get_aliases();               // returns actual mapping
nomask mapping get_environment();           // same thing

/*
 * Unrestricted Public Functions
 */
mixed   get_env(string var);
nomask string  short();
nomask string  query_cap_name();
nomask boolean is_subscribed_chan(string chan);
nomask string* query_channels();
nomask int query_blocking();
nomask int save_player();
nomask varargs int move_player(mixed dest, string dir); // for a pretty move
nomask object  query_connection();
string* query_path();

/*
 * Private Functions
 */
private int set_name(string username);    // override of living.c for security
private int restore_player();
private int cmd_hook(string args);

/*
 * This player object is meant to be shadowed by a shell.
 * The shell handles all user input, variable/alias expansion, look & feel,
 * setting/clearing env variables, etc.
 *
 * Required shell functions:
 * shell_id()    // returns the id of the shell
 *
 * Optional shell functions:
 * shell_init()  // called by LOGIN_D to initialize shell; cannot use create()
 *               // since it is 'nomask' in the user ob.
 *
 * Shell calls these local functions (ones that return mappings and arrays
 * will return the actual handles if the shell is the caller, otherwise a
 * copy of the array or mapping is returned):
 * set_env(), set_cwd(), get_aliases(), get_environment()
 *
 * Required environment variables:
 * TITLE, PATH
 *
 * Currently used by user ob, shell and other things:
 * TERM, WIDTH, LENGTH, MSGBUF_SIZE
 * (predicted: MIN, MOUT, MMIN, MMOUT, MCLONE, MHOME, WKROOM, PATH,
 *             TERM (termtype:LENGTHxWIDTH) )
 */

/*
 * Implementation ---------------------------------------------------------
 */

void
initialize()
{
  ::initialize();
  channels = ({ });
  aliases = ([ ]);
  env_vars = ([ ]);
  msgbuf = ({ });
}

int
remove()
{
  if(!query_name()) return 0;

  foreach(string chan in channels) {
    if(CHAT_D->channel_exists(chan))
      CHAT_D->remove_member(chan, this_player());
    else
      channels -= ({ chan });   // channel no longer exists, delete
  }

  if(connection)
    destruct(connection, 1);

  return ::remove();
}

// For resizing of user's window

void
window_size(int w, int h)
{
  env_vars["WIDTH"] = w;
  env_vars["LENGTH"] = h;
}

void
write_prompt()
{
  message(MSG_PROMPT, "no shell> ", this_object());
}

void
write_ed_prompt(int mode)
{
  message(MSG_PROMPT, "ed:", this_object());
}

void
receive_message(string msgclass, string msg)
{
  string ttype = env_vars["TERM"];
  int width = env_vars["WIDTH"];

  if( !ttype ) ttype = DEFAULT_TERM;

  if( !blocking )
  {
    if(msgclass != "nofilter")
      msg = TERMCAP_D->translate(msg, ttype, width);
    receive(msg);
  }
  else
  {
    int size = (env_vars["MSGBUF_SIZE"] ?
		env_vars["MSGBUF_SIZE"]: MAX_MSGBUF_SIZE);
    msgbuf += ({ msg });
    if( sizeof(msgbuf) > size )
      msgbuf = msgbuf[1..];
  }
}

void
receive_snoop(string msg)
{
  receive_message("snoop", "%^L_RED%^] " + msg + "%^RESET%^");
}

nomask void
net_dead()
{
  set_heart_beat(0);           // disable heartbeat

  /*
   * If in ed, quit.
   */
  if(in_edit()) {
    ed_cmd(".");
    ed_cmd("w " + user_cwd(query_name()) + "/ed_crash_file");
    ed_cmd("Q");
  }

  if(environment())
    message("system", query_cap_name() + " is link-dead.\n", environment(),
	    this_player());

  blocking = 1;                // cache messages
}

nomask int
move_or_destruct(object dest)
{
  this_object()->move(VOID_OB);

  message(MSG_SYS,
	  "The world dissolves around you.  You find yourself floating..\n",
	  this_object());

  return 1;
}

/*
 * Called from LOGIN_D once the player has successfully logged in but before
 * the privs have been set.
 */
nomask void
init_player(string username)
{
  if( file_name(previous_object()) != LOGIN_D )
    return 0;
  
  set_name(username);              // static--won't get zeroed out
  restore_player();                // non-statics are not zeroed out

  // use a call other so a shell can catch this
  add_action((: this_object()->cmd_hook($1) :), "");

  foreach(string chan in channels) {
    if(CHAT_D->channel_exists(chan))
      CHAT_D->add_member(chan, this_player());
    else
      channels -= ({ chan });   // channel no longer exists, delete
  }
  
  if( get_env("PATH") ) {
    foreach(string path in explode(get_env("PATH"), ":"))
      if( !CMD_D->hashed_path(path) ) CMD_D->hash_path(path);
  }
}

/*
 * also called from login daemon when reconnecting to linkdead player
 */
nomask void
reconnect()
{
  /* security -- admin level */

  set_heart_beat(1);            // re-enable heartbeat
  set_blocking(0);

  message(MSG_SYS, "\nReconnected.\n", this_player());
  say(query_cap_name() + " has reconnected.\n");
}

nomask int
set_cap_name(string capname)
{
  /* security -- admin level */

  cap_name = capname;
}

nomask void
set_channels(string* chans)
{
  /* security -- admin level */
  channels = copy(chans);
}

nomask void
set_blocking(int is_blocking)
{
  /* security -- admin level? */
  blocking = is_blocking;

  if( !is_blocking && sizeof(msgbuf) )
  {
    string* tmp = copy(msgbuf);
    msgbuf = ({ });
    message("system", "These messages were held for you:\n", this_object());
    more(explode(implode(tmp, ""), "\n"));
  }
}

nomask int
set_connection(object ob)
{
  /* security -- admin level */

  connection = ob;
  return 1;
}

nomask int
force_me(string cmd)
{
  /* security -- admin level */

  return command(cmd);
}

int
set_env(string var, mixed val)
{
  /* security -- admin level */
//  if(query_shadowing(previous_object()) != this_object()) return 0;

  if( !val )
    map_delete(env_vars, var);
  else
    env_vars[var] = val;

  return 1;
}

nomask mapping
get_aliases()
{
  /*
   * Protected version
   */
  if(query_shadowing(previous_object()) != this_object())
    return copy(aliases);

  /*
   * Yes, we are returning a handle to the actual mapping, this is to make
   * alias handling in the shell easier. (Can just use mapping.)
   */
  return aliases;
}

nomask mapping
get_environment()
{
  /*
   * Protected version
   */
  if(query_shadowing(previous_object()) != this_object())
    return copy(env_vars);

  /*
   * Yes, we are returning a handle to the actual mapping, this is to make
   * alias handling in the shell easier. (Can just use mapping.)
   */
  return env_vars;
}

mixed
get_env(string var)
{
  return env_vars[var];
}

nomask string
short()
{
  string title = get_env("TITLE");

  if(title) {
    if(strsrch(title, "$N") != -1)
      return replace_string(title, "$N", query_cap_name());
    else
      return sprintf("%s %s", query_cap_name(), title);
  }
  return sprintf("%s the title-less", query_cap_name());
}

nomask string
query_cap_name()
{
  if( !cap_name )
    return capitalize( query_name() );
  else
    return cap_name;
}

nomask boolean
is_subscribed_chan(string chan)
{
  return member_array(chan, channels) != -1;
}

nomask string*
query_channels()
{
  return copy(channels);
}

nomask int
query_blocking()
{
  return blocking;
}

nomask int
save_player()
{
  string file, username = query_name();

  file = USER_BODY_DIR "/" + username[0..0] + "/" + username;

  return save_object(file, 1);                       // save 0 values too
}

nomask varargs int
move_player(mixed dest, string dir)
{
  object from = environment();

  if(!dest) return 0;

  if(stringp(dest))
    sscanf(dest, "%s#%s", dir, dest);

  if( move(dest) > 0 ) {
    tell_room(from, sprintf("%s moves to the %s.\n", query_cap_name(), dir));
    say(sprintf("%s arrives.\n", query_cap_name()));
    command("look");
    return 1;
  }
  return 0;
}

nomask object
query_connection()
{
  return connection;
}

private int
set_name(string username)
{
  ::set_name(username);
}

private int
restore_player()
{
  string username = query_name();
  string file;

  file = USER_BODY_DIR "/"+ username[0..0] + "/" + username;

  // don't 0 out non-statics
  return restore_object(file, 1);
}

int
cmd_hook(string arg)
{
  string verb = query_verb();
  object cmd_ob;

  if(query_shadowing(previous_object()) != this_object() &&
     previous_object() != this_object()) return 0;

  if(!strlen(arg)) arg = 0;

  if( cmd_ob = CMD_D->find_cmd(verb) )
    return (int) cmd_ob->_main(arg);

  // if( EMOTE_D->cmd_emote(verb, arg) ) return 1;
  return 0;
}

//  start_ed:     Starts the editor.
//      file      full path of the file to edit.
//      restrict  if this is 1, the commands to modify the file are disabled

nomask int
start_ed(string file, int restrict)
{
  int size = file_size(file);

  if(size == -2) return 0;                       // don't edit directories

  message(MSG_ED,
	  sprintf("%s%s\n", file, (size > 0 ? ": " + size + " bytes." : "")),
	  this_player());
  message(MSG_ED, ed_start(file, restrict), this_player());

  this_object()->write_ed_prompt(query_ed_mode()); // call other so can catch
                                                   // with a shadow
  input_to("process_ed_input");
  return 1;
}

nomask void
process_ed_input(string input)
{
  int md;
  message(MSG_ED, ed_cmd(input), this_player());

  
  if( (md = query_ed_mode()) != -1) {
    this_object()->write_ed_prompt(md);            // call other so can catch
                                                   // with a shadow
    input_to("process_ed_input");
  }
}

string*
query_path()
{
  return DEFAULT_PATHS;
}
