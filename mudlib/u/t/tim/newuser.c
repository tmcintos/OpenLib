/*  -*- LPC -*-  */
// file:  user.c
// mudlib:  UltraLib
// purpose:  an interactive object representing the player's body
//           intended to interface with a shell to do command processing
// Originally written by Tim 2/16/96

#include <mudlib.h>
#include <dirs.h>
#include <login.h>
#include <uid.h>
#include <object_types.h>
#include <daemons.h>

#define MSG_ED "ed"
#define MSG_PROMPT "prompt"
#define MSG_SYS "system"

inherit LIVING;
inherit INHERIT_DIR "/more";

private static object connection;   // connection object pointer
private static object shell;        // shell pointer
private static string current_dir;  // current working directory

private string  cap_name;           // name with proper capitalization
private mapping env_vars;           // environment variable mapping
private mapping aliases;            // alias mapping
private string* channels;           // list of subscribed channels

/*
 * Applies
 */
void   create();
void   remove();
string process_input(string input);
void   write_prompt();
void   receive_message(string msgclass, string msg);
void   net_dead();
int    move_or_destruct(object dest);

/*
 * Restricted Public Functions
 */
void    init_player(string username);    // called from logind sets name etc.
void    reconnect();                     // also called from login daemon
int     set_cap_name(string capname);
void    set_channels(string* chans);
int     set_connection(object ob);
int     force_me(string cmd);
int     start_ed(string file, int rst);  // input handled by process_input()
void    process_ed_input(string input);
void    set_env(string var, mixed val);
int     set_cwd(string cwd);
mapping get_aliases();                   // returns actual mapping

/*
 * Unrestricted Public Functions
 */
mixed   get_env(string var);
string  query_cwd();
string  short();
string  query_cap_name();
boolean is_subscribed_chan(string chan);
int     save_player();

/*
 * Private Functions
 */
private int set_name(string username);    // override of living.c for security
private int restore_player();
private int cmd_hook(string args);

/*
 * The shell handles all user input, variable/alias expansion, look & feel,
 * setting/clearing env variables, etc.
 *
 * Required functions in a shell:
 *
 * string shell_prompt();                            // return normal prompt
 * string shell_ed_prompt();                         // return prompt for ed
 * string shell_process_input(string input);         // process player input
 * string shell_filter_message(string msgclass, string msg); // filter msgs
 * int    shell_cmd_hook(string verb, string arg);   // shell can catch cmds
 *
 * Shell (and only shell) calls these local functions:
 * set_env(), set_cwd(), get_aliases()
 *
 * Required environment variables:
 * TITLE
 *
 * (predicted: MIN, MOUT, MMIN, MMOUT, MCLONE, MHOME, WKROOM, PATH,
 *             TERM (termtype:LENGTHxWIDTH) )
 */

/*
 * Implementation ---------------------------------------------------------
 */

void create()
{
  living::create();
  living::set_object_class(query_object_class() | OBJECT_PLAYER);

  seteuid(0);                             // so logind can export_uid() to us

  channels = ({});
  aliases = ([]);
  env_vars = ([]);
}

int remove()
{
  save_player();
  foreach(string chan in channels)
    CHAT_D->remove_member(chan, this_player());

  if(connection) {
    connection->save_connection(query_name());
    efun::destruct(connection);
  }

  living::remove();
}

string process_input(string input)
{
  return (string) shell->shell_process_input(input);
}

void
write_prompt()
{
  message(MSG_PROMPT, (string) shell->shell_prompt(), this_player());
}

void
receive_message(string msgclass, string msg)
{
  receive((string) shell->shell_filter_message(msgclass, msg));
}

void
net_dead()
{
  set_heart_beat(0);           // disable heartbeat

  /*
   * If in ed, quit.
   */
  if(in_edit()) {
    ed_cmd(".");
    ed_cmd("w " + connection->query_home_dir() + "/ed_crash_file");
    ed_cmd("Q");
  }

  tell_room(environment(), query_cap_name() + " is link-dead.\n");
}

int
move_or_destruct(object dest)
{
  this_object()->move(VOID_OB);

  message(MSG_SYS,
	  "The world dissolves around you.  You find yourself floating..\n",
	  this_object());

  return 1;
}

/*
 * Called from LOGIN_D once the player has successfully logged in and
 * the uid has been set.
 */
void init_player(string username)
{
  seteuid(getuid(this_object()));
  set_name(username);              // static--won't get zeroed out

  restore_player();                // non-statics are not zeroed out

  add_action((: cmd_hook :), "");  // command hook add_action

  foreach(string chan in channels)
    CHAT_D->add_member(chan, this_player());
}

/*
 * also called from login daemon when reconnecting to linkdead player
 */
void
reconnect()
{
  if(geteuid(previous_object()) != ROOT_UID) return;

  set_heart_beat(1);            // re-enable heartbeat

  message(MSG_SYS, "\nReconnected.\n", this_player());
  say(query_cap_name() + " has reconnected.\n");
}

int
set_cap_name(string capname)
{
  if(getuid(previous_object()) != ROOT_UID) return 0;

  if(lower_case(capname) == query_name()) {
    cap_name = capname;
    return 1;
  }
  return 0;
}

void
set_channels(string* chans)
{
//  if(geteuid(previous_object()) != ROOT_UID) return;
  channels = copy(chans);
}

int
set_connection(object ob)
{
  if(geteuid(previous_object()) != ROOT_UID) return 0;

  connection = ob;
  return 1;
}

int
force_me(string cmd)
{
  // if(geteuid(previous_object()) != ROOT_UID) return 0;

  return command(cmd);
}

void
set_env(string var, mixed val)
{
  if(previous_object() != shell) return;

  if(!val)
    map_delete(env_vars, var);
  else
    env_vars[var] = val;
}

int
set_cwd(string cwd)
{
  if(previous_object() != shell) return 0;
  
  current_dir = cwd;
  return 1;
}

mapping
get_aliases()
{
  if(previous_object() != shell) return 0;

  /*
   * Yes, we are returning a handle to the actual mapping, this is to make
   * alias handling in the shell easier. (Can just use mapping.)
   */
  return aliases;
}

mixed
get_env(string var)
{
  return env_vars[var];
}

string
query_cwd()
{
  return current_dir;
}

string
short()
{
  return sprintf("%s %s", query_cap_name(), get_env("TITLE"));
}

string
query_cap_name()
{
  return cap_name;
}

boolean is_subscribed_chan(string chan)
{
  return member_array(chan, channels) != -1;
}

int
save_player()
{
  string file, dir, username = query_name();

  dir = USER_BODY_DIR "/" + username[0..0];
  file = dir + "/" + username;

  /* create directory if needed */
  if(file_size(dir) == -1) {
    if(!mkdir(dir))
      error("couldn't create directory "+ dir +"\n");
  }

  connection->save_connection(username);         // also save connection
  return save_object(file, 1);                   // save 0 values too
}

private
int
set_name(string username)
{
  living::set_name(username);
}

private
int
restore_player()
{
  string username = query_name();
  string file;

  file = USER_BODY_DIR "/"+ username[0..0] + "/" + username;

  return restore_object(file, 1);          // don't zero out non-statics
}

private
int
cmd_hook(string arg)
{
  string verb = query_verb();
  object cmd_ob;

  if(!strlen(arg)) arg = 0;

  if( shell->shell_cmd_hook(verb, arg) ) return 1;

  if( cmd_ob = CMD_D->find_cmd(verb) ) return (int) cmd_ob->_main(arg);

  // if( EMOTE_D->cmd_emote(verb, arg) ) return 1;

  return 0;
}

//  start_ed:     Starts the editor.
//      file      full path of the file to edit.
//      restrict  if this is 1, the commands to modify the file are disabled

int
start_ed(string file, int restrict)
{
  int size = file_size(file);

  if(size == -2) return 0;                       // don't edit directories

  message(MSG_ED,
	  sprintf("%s%s\n", file, (size > 0 ? ": " + size + " bytes." : "")),
	  this_player());
  message(MSG_ED, ed_start(file, restrict), this_player());
  message(MSG_PROMPT, (string) shell->shell_ed_prompt(), this_player());

  input_to("process_ed_input");
  return 1;
}

void
process_ed_input(string input)
{
  message(MSG_ED, ed_cmd(input), this_player());

  switch(query_ed_mode()) {
  case -2 :    // at --more-- prompt in help screen
    break;
  case -1 :    // done (not in ed)
    return;
  case 0 :     // at the ed prompt
    message(MSG_PROMPT, (string) shell->shell_ed_prompt(), this_player());
    break;
  }

  input_to("process_ed_input");
}
