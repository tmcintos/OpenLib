/*  -*- LPC -*-  */
// example monster...really just testing for Tim right now :)
#include <mudlib.h>

inherit MONSTER "/human";

void
create()
{
  object weap;

  ::create();
  set_name("beavis");
  set_short("Beavis");
  set_long("A scrawny, stupid looking high school kid.\n");
  set_move_around(15);
  set_talk(5, ({"This sucks!",
	     "Hey Butthead!  You suck!",
	     "FIRE...FIRE...FIRE",
	     "Fire's cool.",
	     "Shutup Butthead!!",
	     "I gotta go to the bathroom.",
	   }));
  weap = new("/obj/examples/weapon");
  weap->move(this_object());
  do_wield(weap, 1, 1);
}
