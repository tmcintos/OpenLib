/*  -*- LPC -*-  */
// cmdd.c:  Command Daemon
// Written by Tim fall 1995
//
// 02/03/96  Tim  Optimized a bit
// 02/19/96  Tim  Optimized a bit more :)

#include <mudlib.h>

#define DEBUG
#define SAVE_COMD SECURE_DAEMON_SAVE_DIR "/comd"

inherit DAEMON;

private mapping cmdpathmap;
// the keys of this mapping are the command directories; the values are the
// names of the commands in those directories.

void
create()
{
  cmdpathmap = ([]);
  restore_object(SAVE_COMD, 1);
}

void
reset()
{
  foreach(string path, string* cmdset in cmdpathmap) {
    if( cmdset == ({}) )
      map_delete(cmdpathmap, path);
  }
  // Assert: directories with no commands have been removed from mapping

  save_object(SAVE_COMD);
}

#ifdef DEBUG
string
dump_cmdpathmap()
{
  return dump_variable(cmdpathmap);
}
#endif /* ifdef DEBUG */

varargs
mixed
find_cmd(string name, int flag)
// pre: if name is a path to a file it is already in absolute form
// modifies: nothing
// post: if !flag, returns the command object for the command 'name', else
//       returns the full path to the command 'name'
//       returns 0 if not found or failed to load
{
  string path;
  object ret;
  int found;

  // Check for an absolute path first
  // User should resolve this path before calling this function
  if(!strsrch(name, '/')) {

    /* wiz check here */

    if(!file_exists(name + ".c")) return 0;

    path = "";
    found = 1;
  } else {
    string* paths = (string *)this_player()->query_path();

    // Search each path of the user's path setting until we find
    // (or don't find) the command
    foreach(path in paths) {
      if(path[<1] != '/')
	path += "/";
      // Assert:  path ends with '/'
      
      if(!cmdpathmap[path])
	continue;

      if(member_array(name, cmdpathmap[path]) != -1) {
	found = 1;
	break;
      }
    }
  }
  // Assert: found == 0 if not found, else
  //         path' == first full path found to command 'name'

  if(!found) return 0;
  
  if( catch(ret = load_object(path + name)) ) {
    log_file("cmdd", sprintf("turned off %s\n", path + name));
    if(strlen(path)) cmdpathmap[path] -= ({ name });
    return 0;
  }
  // Assert: 'ret' is the commmand object.  load_object() finds it if it was
  //         already loaded.

  if(!flag)
    return ret;                        // returns command object
  else
    return path + name + ".c";         // returns full path
}

boolean
hashed_path(string path)
// pre: path != 0
// modifies: nothing
// post: returns true if path has been hashed
{
  if(path[<1] != '/')
    path += "/";
  // Assert:  path ends with '/'

  return cmdpathmap[path] != 0;
}

int
hash_path(string path)
// pre: path is not 0
// modifies: cmdpathmap, save file
// post: if returns 0 path is a bad directory, else
//       cmdpathmap[path] is an array of commands in that dir
{
  string* files;

  if(path[<1] != '/')
    path += "/";
  // Assert:  path ends with '/'

  if(!directory_exists(path)) return 0;         // make sure dir exists

  files = get_dir(path);

  cmdpathmap[path] = map(filter(files, (: $1[<2..<1] == ".c" :)),
			 (: $1[0..<3] :));
  // Assert:  cmdpathmap[path] now contains an array of all cmds
  //          in that dir (without the .c extension)

  save_object(SAVE_COMD);
  return 1;
}
