/*  -*- LPC -*-  */
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
  seteuid(getuid(this_object()));
  cmdpathmap = ([]);
  restore_object(SAVE_COMD, 1);
}

void
reset()
{
  string path;

  foreach(path in keys(cmdpathmap)) {
    if(cmdpathmap[path] == ({ }))
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
// pre: true
// modifies: nothing
// post: if !flag, returns the command object for the command 'name', else
//       returns the full path to the command 'name'
//       returns 0 if not found or failed to load
{
  object ret;
  string* paths;
  string path = 0;
  int i;

  paths = (string *)this_player()->query_path();
  
  for(i = 0; i < sizeof(paths); i++) {
    if(paths[i][<1] != '/')
      paths[i] += "/";

    if(member_array(paths[i], keys(cmdpathmap)) == -1)
      continue;

    if(member_array(name, cmdpathmap[paths[i]]) != -1) {
      path = paths[i] + name;
      break;
    }
  }
  // Assert: path == 0 if not found, else
  //         path == first full path found to command 'name'
  //         paths[i] == directory 'name' is in

  if(!strsrch(name, '/') || !strsrch(name, "./")) { // wiz check here
    name = RESOLVE_PATH(name + ".c");
    if(file_exists(name))
      return name;
    else
      return 0;
  }

  if(!path) return 0;         // not found

  if(catch(ret = load_object(path))) {
    log_file("cmdd", sprintf("turned off %s\n", path));
    cmdpathmap[paths[i]] -= ({ name });
    return 0;
  }
  // Assert: 'ret' is the commmand object.  load_object() finds it if it was
  //         already loaded.

  if(!flag)
    return ret;                // returns command object
  else
    return path +".c";         // returns full path
}

boolean
hashed_path(string path)
{
  if(path[<1] != '/')
    path += "/";
  // Assert:  path ends with '/'

  return member_array(path, keys(cmdpathmap)) != -1;
}

int
hash_path(string path)
// pre: path is not 0
// modifies: cmdpathmap, save file
// post: if returns 0 path is a bad directory, else
//       cmdpathmap[path] is an array of commands in that dir
{
  string* files;
  string file;

  if(path[<1] != '/')
    path += "/";
  // Assert:  path ends with '/'

  if(!directory_exists(path))
    return 0;                 // make sure dir exists

  if(member_array(path, keys(cmdpathmap)) == -1)
    cmdpathmap += ([ path : ({ }) ]);
  else
    cmdpathmap[path] = ({});

  files = get_dir(path);

  foreach(file in files) {
    string tmp;

    if(sscanf(file, "%s.c", tmp) == 1)
      cmdpathmap[path] += ({ tmp });
  }
  // Assert:  cmdpathmap[path] now contains an array of all cmds in that dir

  save_object(SAVE_COMD);
  return 1;
}
