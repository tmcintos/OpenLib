/*  -*- LPC -*-  */
#include <command.h>

int
main(string file)
{
  if(!file)
    return notify_fail("usage: ed <filename>\n");

  file = RESOLVE_PATH(file);
  this_player()->start_ed(file);
  return 1;
}
