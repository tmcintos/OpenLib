/*  -*- LPC -*-  */
// Comintern's first LPC code!

#include <mudlib.h>

inherit ROOM;

void
create()
{
  ::create();
  set_light(1);

  set_short("Comintern HQ");
  set_long("You are in the Headquarters of the Comintern.  Dossiers of\n"+
         "various communist agents are scattered around in several small\n"+
         "piles, detailing the work of the international conspiracy.\n"+
         "A huge red flag with the hammer and sickle flys from a flagpole\n"+
         "next to the door, and stirring socialist marches fill the air.\n"+
         "There is a desk on one wall stacked with copies of Das Kapital\n"+
         "and the Manifesto.  Across the room is a door with a sign on it\n"+
         "reading 'Lounge'.  You feel strangly compelled to see what lies\n"+
         "beyond that door...\n");
  set_exits((["start" : "/d/base/start.c",
              "lounge" : "/u/c/comintern/lounge.c",
              "void" : "/adm/obj/daemon/void.c",
	      ]));
}
