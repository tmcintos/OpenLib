/*  -*- LPC -*-  */
// -=Coded by the local idiot Kyricc=-
#include <mudlib.h>

inherit MONSTER "/human";

void
create()
{
  object weap;

  ::create();
  set_name("zombie");
  set_short("Zombie");
  set_long("A badly decayed corpse that has, somehow, come back to life\n");
  set_move_around(15);
  weap = new("/obj/examples/weapon");
  weap->move(this_object());
  do_wield(weap, 1, 1);
}
