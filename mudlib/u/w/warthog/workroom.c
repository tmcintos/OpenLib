/*  -*- LPC -*-  */
// -=: Stolen code by the Warthog :=-

#include <mudlib.h>

inherit ROOM;

void
create()
{
  ::create();
  set_light(1);
  set_short("Warthog's batchelar pad.");
  set_long("This is Warthog's bachelar (spel?) pad.  You see a large\n"+
         "circular bed in the middle of the room.  It is rotating in\n"+
	      "time with the giant mirror ball directly above it.  On the\n"+
	      "bed, among the black silk sheets, you see what appears to \n"+
         "be $200 of pudding (ooohhh yaaaa).  On the felt covered \n"+
         "walls you see various artifacts of the disco era. \n"+
         "In the back you see...can it be...a Cray supercomputer \n"+
         "At the terminal you see Warthog playing another mud..heh.\n");
  set_exits((["start" : "/d/base/start.c",
              "crys" : "/u/k/kyricc/Crysanthium/town_square.c",
              "void" : "/adm/obj/daemon/void.c",
	      ]));
}
