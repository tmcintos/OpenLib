/*  -*- LPC -*-  */
// -=Coded by the local idiot Kyricc=-

#include <mudlib.h>

inherit MONSTER "/human";

void
create()
{
  object weap;

  ::create();
  set_name("rod");
  set_short("Rowdy Rod, the incompetent AOL'er");
  set_long("This is Rod.  He is rowdy.  He logs into America on Line.\n"+
           "Like all AOL'er he is annoying.  He loves to waste the\n"+
           "all precious bandwidth with his mindless rambling.\n");
  set_move_around(15);
  set_talk(5, ({"Woah! Hang ten.  I'm surfing the net!",
	     "Ummm...How do you...",
	     "AOL _IS_ the internet!!!",
	     "Time to leave some more blank usenet news...",
	     "Hey!  Could you mail me a copy of that too?!?",
	     "What's with AOHell?  Dosn't anybody like us?",
	   }));
  weap = new("/obj/examples/weapon");
  weap->move(this_object());
  do_wield(weap, 1, 1);
}

