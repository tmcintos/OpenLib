/*  -*- LPC -*-  */
// cd.c:  change directory;  Tim McIntosh (astasia@iastate.edu)
//        Oversimplified...as always :)  Only needs to be functional
//        right now anyways.

#include <command.h>

int
main(string new_dir)
{

  if(!new_dir)
    new_dir = "~/";

  new_dir = RESOLVE_PATH(new_dir);

  if(file_size(new_dir) != -2)
    return notify_fail("cd: directory does not exist.\n");

  if(!this_player()->set_cwd(new_dir))
    return notify_fail("cd: unable to change cwd.\n");

  return 1;
}
