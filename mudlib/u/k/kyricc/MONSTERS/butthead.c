/*  -*- LPC -*-  */
// -=Coded by the local idiot Kyricc=-
#include <mudlib.h>

inherit MONSTER "/human";

void
create()
{
  object weap;

  ::create();
  set_name("butthead");
  set_short("Butthead");
  set_long("A scrawny, stupid looking high school kid.\n");
  set_move_around(15);
  set_talk(5, ({"This sucks!",
	     "Hey Beavis!  Shut up or I'll slap you.",
	     "That was pretty cool.",
	     "Uh...I think this is a MUD.",
	     "Damn it Beavis!!!",
	     "Huh...uh..huh...",
	     "Shut up!  Fartknocker!",
	     "Huh..He said 'Hole'.  Huh..huh...",
	   }));
  weap = new("/obj/examples/weapon");
  weap->move(this_object());
  do_wield(weap, 1, 0);
}
