/*  -*- LPC -*-  */
// more.c:  Tim McIntosh (astasia@iastate.edu)

#include <command.h>

int
main(string file)
{
  if(!file)
    return notify_fail("usage: more <file>\n");

  file = RESOLVE_PATH(file);

  switch(file_size(file)) {
  case -1 :
    return notify_fail("more: file does not exist.\n");
  case -2 :
    return notify_fail("more: is a directory.\n");
  default :
    this_player()->more(file);
  }

  return 1;
}
