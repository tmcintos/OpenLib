/*  -*- LPC -*-  */
// mudlib:   Lil 
// file:     user.c
// purpose:  is the representation of an interactive (user) in the MUD
//
//  06/26/95  Tim McIntosh:  name is lower case now
//  08/25/95  Tim McIntosh:  put in an 'ed' interface
//  Tim says:  This is quick and dirty...it should be totally rewritten
//  replace this with a functioning version.
//                           added support for keeping track of connection obj
//  09/29/95 Tim McIntosh:  added (object *)query_inventory()
//  09/30/95 Casper:  Added inherits for combat related functions and
//                    quick 'hack' for inheriting 'human' functions into
//                    user.c
//  10/03/95 Tim:  Moved some stuff to living.c, cleaned up some... 

#include <mudlib.h>
#include <uid.h>
#include <daemons.h>
#include <object_types.h>

inherit LIVING;

private string title;                  // our title
private string current_dir;            // current working directory
private string term;                   // terminal type
private mapping aliases;               // our aliases
private function Hook;
private static object connection;      // user's connection object
private string* channels;              // subscribed channels

// search path, take out later
private string* search_path = ({ "/cmd/player",
				 "/adm/cmd/wiz",
				 "/cmd/wiz",
				 "/adm/cmd/adm",
				 "/adm/cmd/player",
				 "/cmd/adm"
			      });

#include "user/more.c"

string query_cwd() { return current_dir; }
string query_term() { return term; }
object query_connection() { return connection; }
string* query_path() { return copy(search_path); }
string* query_channels() { return copy(channels); }

// Overload of living::create()

void
create()
{
  string path;

  living::create();
  living::set_object_class(query_object_class() | OBJECT_PLAYER);
  seteuid(0);                       // so logind can export_uid() to us
  channels = ({ });
  aliases = (["n"   :  "north",
	      "s"   :  "south",
	      "e"   :  "east",
	      "w"   :  "west",
	      "ne"  :  "northeast",
	      "nw"  :  "northwest",
	      "se"  :  "southeast",
	      "sw"  :  "southwest",
	      "i"   :  "inventory",
	      "l"   :  "look",
	      "sc"  :  "score",
	      ]);

  foreach(path in search_path) {
    if(!CMD_D->hashed_path(path) && !CMD_D->hash_path(path))
      search_path -= ({ path });
  }

}

// Overload of living::short()

string
short() { return query_cap_name() + " " + title; }

// Overload of destructor living::remove()

void
remove()
{
  foreach(string chan in channels) {
    CHAT_D->remove_member(chan, this_player());
  }

  if(connection) {
    connection->save_connection(query_name());
    efun::destruct(connection);
  }

  living::remove();
}

// Overload of living::set_name()

int
set_name(string username)
{
  // may wish to add security to prevent just anyone from changing
  // someone else's name.
  if(geteuid(previous_object()) != ROOT_UID)
    return 0;
  living::set_name(username);
}

int
set_title(string ttl)
{
  if(ttl) {
    title = ttl;
  }
  write("You are now "+ short() +".\n");
  return 1;
}

int
set_term(string arg)
{
  term = arg;
}

// set_cwd:  (Tim)  Not sure how to really handle this; this'll do for now...
//          Path resolving should be done in 'cd'
//          returns 0 on fail; 1 on success (as usual i guess)

int
set_cwd(string cwd)
{
  if(!cwd)
    return 0;

  current_dir = cwd;
  return 1;
}

//  Set the connection object for this user

int
set_connection(object ob)
{
  ob->set_active();       // so it won't be destructed so easily
  connection = ob;
  return 1;
}

// Set the list of channels

void
set_channels(string* chans)
{
  channels = chans;
}

boolean
is_subscribed_chan(string chan)
{
  if( member_array(chan, channels) == -1 )
    return FALSE;
  else
    return TRUE;
}

// path_resolve: (Tim) resolve_path is an efun;  This is player specific...

string
path_resolve(string path)
{
  return resolve_path(query_cwd(), path);
}

// possible to modify player input here before driver parses it.

string
process_input(string arg)
{
  mixed* words;
  int i;

  if(!arg || arg == "") return 0;
  if(arg[0] == '\\') return arg[1..];

  words = explode(arg, " ");

  if(member_array(words[0], keys(aliases)) != -1) {
    words[0] = aliases[words[0]];
  }

  if(strsrch(arg, '*') != -1) {
    boolean fail;

    for(i=1; i < sizeof(words); i++) {

      if(words[i] == "*") {
	string tmp;
	if((tmp = query_cwd()) == "/")
	  words[i] = implode(get_dir(tmp), " ");
	else
	  words[i] = implode(get_dir(tmp + "/"), " ");

      } else if(strsrch(words[i], '*') != -1) {
	int j, k;
	string* path;
	string prepath;

	words[i] = path_resolve(words[i]);
	path = explode(words[i], "/");

	for(j=0; j < sizeof(path)-1; j++) {
	  string *tmp;
	  if(sizeof(tmp = get_dir("/"+implode(path[0..j], "/"))) == 1) {
	    path[j] = tmp[0];
	  } else {
	    printf("Ambiguous: /%s\n", implode(path[0..j], "/"));
	    fail = TRUE;
	    break;
	  }
	}

	if(fail) break;

	if(j) {
	  prepath = "/" + implode(path[0..j-1], "/");
	} else {
	  prepath = "";
	}

	if(words[i][<1] == '/')
	  words[i] = get_dir(prepath + "/" + path[j] + "/");
	else
	  words[i] = get_dir(prepath + "/" + path[j]);

	if(!sizeof(words[i])) {
	  printf("%s: no match.\n", arg);
	  fail = TRUE;
	  break;
	}

	for(k = 0; k < sizeof(words[i]); k++)
	  words[i][k] = prepath +"/"+ words[i][k];

	words[i] = implode(words[i], " ");
      }
    } 
  }

  return implode(words, " ");
}

//  Save the values to the playerfile

int
save_player()
{
  string file, dir, username;
  username = query_name();

  dir = DATA_DIR "/user/body/" + username[0..0];
  if(file_size(dir) != -2) mkdir(dir);

  file = dir + "/" + username;

  connection->save_connection(username);
  return save_object(file);
}

//  Restore from playerfile
//  (security here)

int
restore_player()
{
  string username = query_name();
  string file;

  if(!username || username == "") return 0;

  username = lower_case(username);
  file = DATA_DIR "/user/body/"+ username[0..0] + "/" + username;
  return restore_object(file, 1);
}

// setup: used to configure attributes that aren't known by this_object()
// at create() time such as living_name (and so can't be done in create()).

void
setup(string username)
{
  seteuid(getuid(this_object()));

  set_name(username);
  restore_player();

  add_action("commandHook", "", 1);

  if(!channels) {
    channels = ({ });
  } else {
    foreach(string chan in channels) {
      CHAT_D->add_member(chan, this_player());
    }
  }

  if(!title) title = "the utter mudlib hacker";
}

// (Tim) don't know if this should be here...but I'm doing it anyway for now
int
force_me(string command)
{
  // security ?
  command(command);
  return 1;
}

// this is a temp. sloppy hack for now

int
commandHook(string arg)
{
  string verb = query_verb();
  object cobj;
  string aliastmp1;
  string tmp_emote;

    if(arg == "") arg = 0;

//  Get this out of the player....LATER!!!  (Tim)
    switch(verb) {
    case "path":
      if(!arg) { write(implode(search_path, ":") +"\n"); return 1; }
      if(CMD_D->hashed_path(arg) || CMD_D->hash_path(arg)) {
	if(member_array(arg, search_path) == -1) {
	  search_path += ({ arg });
	  write("Path added.\n");
	}
      }
      return 1;
    case "term":
      if(!arg) { write(term +"\n"); return 1; }
      term = arg;
      return 1;
    case "alias":
      if(!arg) { write(dump_variable(aliases) +"\n"); return 1; }
      if(arg && sscanf(arg, "%s %s", aliastmp1, arg) == 2) {
	aliases += ([ aliastmp1 : arg ]);
      } else {
	if(member_array(arg, keys(aliases)) != -1)
	  map_delete(aliases, arg);
      }
      return 1;
    }

    if(cobj = CMD_D->find_cmd(verb)) {
      return (int) cobj->main(arg);
    } else {
      tmp_emote = verb;

      if(arg)
	tmp_emote += " " + arg;

      return (int) EMOTE_D->emote_search(verb, tmp_emote);
    }
}

// init: called by the driver to give the object a chance to add some
// actions (see the MudOS "applies" documentation for a better description).

void
init()
{
	// using "" as the second argument to add_action() causes the driver
	// to call commandHook() for those user inputs not matched by other
	// add_action defined commands (thus 'commandHook' becomes the default
	// action for those verbs without an explicitly associated action).
	if (this_object() == this_player()) {
		add_action("commandHook", "", 1);
	}
}

// receive_message: called by the message() efun.

void
receive_message(string msgClass, string msg)
{
	// the meaning of 'msgClass' is at the mudlib's discretion
//	receive(wrap(termcap_format_line(msg, query_term())));
	receive(termcap_format_line(msg, query_term()));
}

// net_dead: called by the gamedriver when an interactive player loses
// hir network connection to the mud.

void
net_dead()
{
  set_heart_beat(0);

#ifndef __HAS_ED__
  if(in_edit()) {
    ed_cmd(".");
    ed_cmd("w "+ connection->query_home_dir() +"/ed_crash_file");
    ed_cmd("Q");
  }
#endif

  tell_room(environment(), query_cap_name() + " is link-dead.\n");
}

// reconnect: called by the login.c object when a netdead player reconnects.

void
reconnect()
{
	set_heart_beat(1);
    write("\nReconnected.\n");
    say(query_cap_name() + " has reconnected.\n");
}

// tim: this needs to be here so you don't get destructed when the room is...

int
move_or_destruct(object dest)
{
  this_object()->move(VOID_OB);
  tell_object(this_object(),
	  "The world dissolves around you.  You find yourself floating..\n");
  return 1;
}


//  start_ed:     Starts the editor.
//      file      full path of the file to edit.
//      restrict  if this is 1, the commands to modify the file are disabled
//
//  NOTE:  This should stay in the player object.

int
start_ed(string file, int restrict)
{
#ifdef __HAS_ED__  // old way
  ed(file, "", restrict);
  return 1;
}
#else  // v21 crap
  string blurb;
  int tmp;

  tmp = file_size(file);
  

  write(file + ((tmp > 0) ?
		": " + tmp + " bytes.\n" :
		"\n"));
  write(ed_start(file, restrict));
  write(": ");
  input_to("process_ed_input");
  return 1;
}

void
process_ed_input(string input)
{
  int mode;

  write(ed_cmd(input));
  mode = query_ed_mode();

  switch(mode) {
  case -2 :    // at --more-- prompt in help screen
    break;
  case -1 :    // not in ed
    return;
  case 0 :     // at the ed prompt
    write(": ");
    break;
  }
  input_to("process_ed_input");
}
#endif
