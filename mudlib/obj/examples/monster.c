/*  -*- LPC -*-  */
// example monster...really just testing for Tim right now :)
#include <mudlib.h>
#include <gender.h>

inherit MONSTER "/human";

void
setup()
{
  object weap;

  set_name("beavis");
  add_ids(({ "kid" }));
  set_short("Beavis");
  set_long("A scrawny, stupid looking high school kid.\n");
  set_gender(GENDER_MALE);
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
  do_wield(weap, 1, 0);
}
