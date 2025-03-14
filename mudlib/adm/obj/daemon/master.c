//  -*- LPC -*-
//
// NOTE: This file is NOT covered by the OpenLib License agreement
//

#include <mudlib.h>
#include <daemons.h>
#include <login.h>

#include "master/valid.c"

void
create()
{
  efun::set_privs(this_object(), save_variable(1));
}

object
connect(int portnum)
{
  object user;
  mixed err;
  
  err = catch( user = new(CONNECTION_OB) );
  
  if( err )
  {
    message("system",
	    "Someone must be working on the connection object.\n",
	    this_player());
    destruct(user);
  }

  return user;
}

// compile_object: This is used for loading MudOS "virtual" objects.
// It should return the object the mudlib wishes to associate with the
// filename named by 'file'.  It should return 0 if no object is to be
// associated.

mixed
compile_object(string file)
{
  //	return (mixed)VIRTUAL_D->compile_object(file);
  return 0;
}

// This is called when there is a driver segmentation fault or a bus error,
// etc.  As it's static it can't be called by anything but the driver (and
// master).

static void
crash(string error, object command_giver, object current_object)
{
  message("system",
	  "Master object shouts: Damn!\n"
	  "Master object tells you: "
	  "Buckle up bonehead; you're going for a ride!\n"
	  "Master object tells you: The game is crashing.\n",
	  users());

  log_file("crashes", sprintf("%s crashed on: %s, error: %s\n",
			      mud_name(), ctime(time()), error));
  if( command_giver )
    log_file("crashes", "this_player: " + file_name(command_giver) + "\n");
  if( current_object )
    log_file("crashes", "this_object: " + file_name(current_object) + "\n");
}

// Function name:   update_file
// Description:     reads in a file, ignoring lines that begin with '#'
// Arguements:      file: a string that shows what file to read in.
// Return:          Array of nonblank lines that don't begin with '#'
// Note:            must be declared static (else a security hole)

static string*
update_file(string file)
{
  string *array;
  string str;

  str = read_file(file);

  if (!str) return ({});

  array = explode(str, "\n");

  for (int i = 0; i < sizeof(array); i++) {
    if (array[i][0] == '#')
      array[i] = 0;
  }

  return array;
}

// Function name:       epilog
// Return:              List of files to preload

string*
epilog(int load_empty)
{
  return update_file(CONFIG_DIR + "/preload");
}

// preload an object

void
preload(string file)
{
  int t1;
  string err;

  if( !file_exists(file + ".c") )
    return;

  t1 = time();
  debug_message("Preloading : " + file + "...");

  if( err = catch(call_other(file, "??")) )
  {
    debug_message("\nError " + err + " when loading " + file + "\n");
  }
  else
  {
    t1 = time() - t1;
    debug_message("(" + t1/60 + "." + t1 % 60 + ")\n");
  }
}

// Write an error message into a log file. The error occured in the object
// 'file', giving the error message 'message'.

int
error_handler(mapping error, int caught)
{
  string name, home;
  string line;

  if( caught ) return;

  line = sprintf("\n[ERROR] %s"
		 "  time   : %s\n"
		 "  file   : %s:%d\n"
		 "  pgm/obj: %s (%O)\n"
		 "  [TRACE]  size==%d\n",
		 error["error"],
		 ctime(time()),
		 error["file"], error["line"],
		 error["program"], error["object"],
		 sizeof(error["trace"]));

  foreach(mapping traceback in error["trace"])
  {
    line += sprintf("    file   : %s:%d\n"
		    "    pgm/obj: %s (%O)\n"
		    "    func   : %s (",
		    traceback["file"], traceback["line"],
		    traceback["program"], traceback["object"],
		    traceback["function"]);
    if( arrayp(traceback["arguments"]) )
    {
      string paramlist = "";

      foreach(mixed arg in traceback["arguments"])
	paramlist += sprintf("%O, ", arg);

      line += break_string(replace_string(paramlist, "\n", ""), 60, 13);
    }
    else
      line += "void";

    line += ")\n";
    line += sprintf("    %*'-'s", 70, "\n");
  }

  name = file_owner(error["file"]);

  if( name )
    home = user_cwd(name);
  else
    home = LOG_DIR;
  home = home + "/errors";

  if( this_interactive() )
    message("system", "\nruntime: error: "
	    "Check " + home + "/runtime for more information.\n",
	    this_interactive());
   
  if( directory_exists(home) )
    unguarded((: write_file, home + "/runtime", line :), 1);

  return 1;
}

void
log_error(string file, string message)
{
  string name, home;

  message = break_string(replace_string(message, "\n", ""), 79) + "\n";

  name = file_owner(file);

  if( name )
    home = user_cwd(name);
  else
    home = LOG_DIR;
  home = home + "/errors";

  if( this_interactive() )
    message("system", "\ncompile: error: "
	    "Check "+ home +"/compile for more information.\n",
	    this_interactive());

  if( directory_exists(home) )
    unguarded((: write_file, home + "/compile", message :), 1);
}

// save_ed_setup and restore_ed_setup are called by the ed to maintain
// individual options settings. These functions are located in the master
// object so that the local admins can decide what strategy they want to use.

int
save_ed_setup(object who, int code)
{
  string file;
  
  if( !intp(code) )
    return 0;

  file = user_path(who->query_name()) + ".edrc";
  rm(file);
  return unguarded((: write_file, file, code + "" :), 1);
}

// Retrieve the ed setup. No meaning to defend this file read from
// unauthorized access.

int
retrieve_ed_setup(object who)
{
  string file;
  int code;
  
  file = user_path(who->query_name()) + ".edrc";
  if( file_size(file) <= 0 )
  {
    return 0;
  }
  sscanf(read_file(file), "%d", code);
  return code;
}

// When an object is destructed, this function is called with every
// item in that room.  We get the chance to save users from being destructed.

void
destruct_environment_of(object ob)
{
  if( !interactive(ob) )
    return;

  tell_object(ob, "The object containing you was dested.\n");
  ob->move(VOID_OB);
}

// make_path_absolute: This is called by the driver to resolve path names in ed

string
make_path_absolute(string file)
{
  return absolute_path((string)this_player()->query_cwd(), file);
}

string
privs_file(string str)
{
  return (string)call_other(SECURITY_D, "privs_file", str);
}

// Returns the list of valid literals that may appear in parse rules
// Of course this should be generalized *wink* -Tim 10/16/96

string*
parse_command_prepos_list()
{
  return ({ "at", "in", "on", "by", "from", "into", "of" });
}
