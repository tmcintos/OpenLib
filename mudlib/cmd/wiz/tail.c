/*  -*- LPC -*-  */
// tail.c:  Tim McIntosh (astasia@iastate.edu)

#include <command.h>

int
main(string file)
{
  if(!file)
    return notify_fail("usage: tail <file>\n");

  file = RESOLVE_PATH(file);

  if (!tail(file))
    return notify_fail("tail: file does not exists or isn't readable.\n");

  return 1;
}
