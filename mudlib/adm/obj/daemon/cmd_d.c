//  -*- LPC -*-
// cmdd.c:  Command Daemon
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
// Written by Tim fall 1995
//
// 02/03/96  Tim  Optimized a bit
// 02/19/96  Tim  Optimized a bit more :)
// 09/05/96  Tim  Added necessary unguarded()'s
//

#include <mudlib.h>
#include <save.h>

#define DEBUG

inherit DAEMON;

private mapping cmdpathmap;
// the keys of this mapping are the command directories; the values are the
// names of the commands in those directories.

void
create()
{
  cmdpathmap = ([]);
  unguarded((: restore_object(SAVE_CMD_D, 1) :), 1);
}

void
reset()
{
  foreach(string path, string* cmdset in cmdpathmap) {
    if( cmdset == ({}) )
      map_delete(cmdpathmap, path);
  }
  // Assert: directories with no commands have been removed from mapping

  unguarded((: save_object(SAVE_CMD_D) :), 1);
}

#ifdef DEBUG
string
dump_cmdpathmap()
{
  return dump_variable(cmdpathmap);
}
#endif /* ifdef DEBUG */

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

  unguarded((: save_object(SAVE_CMD_D) :), 1);
  return 1;
}

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

      if( !hashed_path(path) )
	hash_path(path);
      
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
