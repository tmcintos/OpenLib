/*  -*- LPC -*-  */
// comd.c:  command daemon
//   Orig by Regional LPC guru Tim

#include <mudlib.h>

inherit DAEMON;

private mapping commands = ([]);
private string* command_paths = ({});

int
hash(string cmd)
{
  command_paths = keys(commands);
}

object
find_cmd_ob(string verb)
{
  string* paths = this_player()->query_path();
  string path;

  foreach(path in paths) {
    int idx = 0;

    // add new path to hash table if it exists
    if(member_array(path, command_paths) == -1) {
      if(directory_exists(path)) {
	commands += ([ path : ({ }) ]);
	command_paths += ({ path });
      } else {
	continue;
      }
    }

    // if command is already found in hash table return it...otherwise
    // add new entry
    if((idx = member_array(verb, commands[path])) == -1) {
      if(file_exists(path +"/"+ verb +".c")) {
	commands[path] += ({ verb });
      } else {
	continue;
      }
