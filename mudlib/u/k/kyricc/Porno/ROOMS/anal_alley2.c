/* -*- LPC -*- */
// -= Coded by the local idiot Kyricc =- //

#include <mudlib.h>

inherit ROOM;

void
create()
{

  ::create();

  set_light(1);
  set_short("The west end of Anal Alley\n");
  set_long(@ENDLONG
You are on a trash strewen alleyway.  The alley leads east and west
from here.
ENDLONG);

  set_exits(([ "east" : "/u/k/kyricc/Porno/ROOMS/lewed_lane1n.c",
               "west" : "/u/k/kyricc/Porno/ROOMS/anal_alley1.c",
           ]));

}
