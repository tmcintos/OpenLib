#include <mudlib.h>

inherit MONSTER "/human";

void
create()
{
  object weap;

  ::create();
  set_name("Slice");
  set_short("Slice, the mandarin orange guard");
  set_long("A mean looking guard that looks very orange!\n");
  set_move_around(15);
  set_talk(5, ({"I'm a guard!",
             "If you mess with me, you die!",
             "Don't mess with me CHICKEN!",
             "Grrrrrrrrr!!",
             "I will crush everybody!",
             "I will rule the world!",
           }));
  weap = new("/obj/examples/weapon");
  weap->move(this_object());
  do_wield(weap, 1, 1);
}
