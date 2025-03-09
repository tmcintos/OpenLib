/*  -*- LPC -*-  */
// file: /daemon/master.c

#include <mudlib.h>
#include <daemons.h>

#include "master/valid.c"      // all the valid_* functions

object
connect(int portnum)
{
  object user;
  mixed err;
  
  err = catch( user = new(CONNECTION_OB) );
  
  if (err) {
    efun::write("It looks like someone is working on the connect object.\n");
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
  shout("Master object shouts: Damn!\n");
  shout("Master object tells you: "
	"Buckle up bonehead; you're going for a ride!\n");
  shout("Master object tells you: The game is crashing.\n");

  log_file("crashes", sprintf("%s crashed on: %s, error: %s\n",
			      mud_name(), ctime(time()), error));
  if (command_giver)
    log_file("crashes", "this_player: " + file_name(command_giver) + "\n");
  if (current_object)
    log_file("crashes", "this_object: " + file_name(current_object) + "\n");
}

// Function name:   update_file
// Description:     reads in a file, ignoring lines that begin with '#'
// Arguements:      file: a string that shows what file to read in.
// Return:          Array of nonblank lines that don't begin with '#'
// Note:            must be declared static (else a security hole)

static string *
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

string *
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

  if (!file_exists(file + ".c")) return;

  t1 = time();
  efun::write("Preloading : " + file + "...");
  err = catch(call_other(file, "??"));
  if (err != 0) {
    efun::write("\nError " + err + " when loading " + file + "\n");
  } else {
    t1 = time() - t1;
    efun::write("(" + t1/60 + "." + t1 % 60 + ")\n");
  }
}

// Write an error message into a log file. The error occured in the object
// 'file', giving the error message 'message'.

int
error_handler(mapping error)
{
  string name, home;
  string line;
  mapping *trace = error["trace"];

  line = sprintf("\n[ERROR] %s"
		 "  obj : (%O)\n"
		 "  file: %s:%d\n"
		 "  prgm: %s\n",
		 error["error"], error["object"],
		 error["file"], error["line"], error["program"]);

  if(sizeof(trace)) {
    line += sprintf("  func: %s\n"
		    "  args: ( ", 
		    trace[sizeof(trace)-1]["function"]);

    if(arrayp(trace[sizeof(trace)-1]["arguments"])) {
      foreach(mixed arg in trace[sizeof(trace)-1]["arguments"])
	line += sprintf("%O, ", arg);
    } else
      line += sprintf("%O", trace[sizeof(trace)-1]["arguments"]);

    line += ")\n";
  }

  if(this_player(1)) {
    tell_object(this_player(1), sprintf("runtime: file: %s:%d;  %s",
					error["file"], error["line"],
					error["error"]));
  }
   
  name = file_owner(error["file"]);
  if (name)
    home = user_cwd(name);
  else
    home = LOG_DIR;
  home = home + "/errors";

  if(directory_exists(home))
    write_file(home + "/runtime", line);

  return 1;
}

void
log_error(string file, string message)
{
  string name, home;
   
  name = file_owner(file);
  if (name)
    home = user_cwd(name);
  else
    home = LOG_DIR;
  home = home + "/errors";

  tell_object(this_player(1),"\ncompile: " + message);

  if(directory_exists(home))
    write_file(home + "/compile", message);
}

// save_ed_setup and restore_ed_setup are called by the ed to maintain
// individual options settings. These functions are located in the master
// object so that the local admins can decide what strategy they want to use.

int
save_ed_setup(object who, int code)
{
  string file;
  
  if (!intp(code)) return 0;

  file = user_path(who->query_name()) + ".edrc";
  rm(file);
  return write_file(file, code + "");
}

// Retrieve the ed setup. No meaning to defend this file read from
// unauthorized access.

int
retrieve_ed_setup(object who)
{
  string file;
  int code;
  
  file = user_path(who->query_name()) + ".edrc";
  if (file_size(file) <= 0) {
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
  if (!interactive(ob)) return;

  tell_object(ob, "The object containing you was dested.\n");
  ob->move(VOID_OB);
}

// make_path_absolute: This is called by the driver to resolve path names in ed

string
make_path_absolute(string file)
{
  file = absolute_path((string)this_player()->query_cwd(), file);
  return (string)call_other(SIMUL_EFUN, "absolute_path",
			    this_player()->query_cwd(), file);
}

string
privs_file(string str)
{
  return (string)call_other(SIMUL_EFUN, "privs_file", str);
}

string
creator_file(string str)
{
  return (string)call_other(SIMUL_EFUN, "creator_file", str);
}

string
domain_file(string str)
{
  return (string)call_other(SIMUL_EFUN, "domain_file", str);
}

string
author_file(string str)
{
  return (string)call_other(SIMUL_EFUN, "author_file", str);
}
