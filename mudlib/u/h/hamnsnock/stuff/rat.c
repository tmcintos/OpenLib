#include <mudlib.h>

inherit MONSTER "/human";

void
create()
{
  object weap;

  ::create();
  set_name("Rat");
  set_short("Large ugly rat");
  set_long("You look at the rat, and see that it is infact very large\n");
  set_move_around(15);
}
