//Basic kill command.  Orig by Casper 10/14/95
#include <daemons.h>
#include <command.h>
 
int main(string str)
{
  object victim;
  int foo;
 
  if(!str)
    return notify_fail("Kill what?\n");
  if(!(victim = present(str,environment(this_player()))))
    return notify_fail("There is no "+str+" here.\n");
  foo = COMBAT_D->kill_ob(this_player(),victim);
  //Returns:
  // 1 for non-living target
  // 2 for attacking self
  // 3 for unallowed combat
  // 4 for victim not present
  // 0 on success
  if(foo == 1)
    return notify_fail("You can't attack that which does not fight.\n");
  if(foo == 2)
    return notify_fail("You don't REALLY want to kill yourself.\n");
  if(foo == 3)
    return notify_fail("You can't bring yourself to do that.\n");
  if(foo == 4)
    return notify_fail("You can if you want, but they ain't about.\n");
  return 1;
}
