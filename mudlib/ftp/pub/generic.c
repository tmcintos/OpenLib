/*  -*- LPC -*-  */
// -= Coded by the local idiot Kyricc=-

#include <mudlib.h>

inherit ROOM;

void
create()
{

  ::create();

  set_light(1);
  set_short("This is the Porno workroom.\n");
  set_long(@ENDLONG
This is where jizz-master Kyricc creates porn-land.  You see many sex toys
strewn about the lab.  All kinds of different motion lotions and jellies.
As you move on you notice the floor is very sticky.
ENDLONG);

set_exits((["start" : "/u/d/base/start.c",
            "boob" : "/u/k/kyricc/Porno/ROOMS/anal_alley1.c",
	      ]));
}
