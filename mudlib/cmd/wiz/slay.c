#include <command.h>

int
main(string str)
{
  object victim;

  if(!(victim = present(str,environment(this_player()))))
    return notify_fail("There is no "+str+" here.\n");

  victim->die();

  return 1;
}
