/*  -*- LPC -*-  */
// tail.c:  Tim McIntosh (astasia@iastate.edu)
//
// 04.13.96  Tim:  fixed to prevent tail'ing directories and typo in error msg

#include <command.h>

int
main(string file)
{
  if(!file)
    return notify_fail("usage: tail <file>\n");

  file = RESOLVE_PATH(file);

  if ( !file_exists(file) )
    return notify_fail("tail: file does not exist.\n");
  else
    tail(file);

  return 1;
}
