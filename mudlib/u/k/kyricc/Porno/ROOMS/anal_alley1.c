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
You are on a trash strewen alleyway.  The alley leads east from here.
ENDLONG);

  set_exits(([ "east" : "/u/k/kyricc/Porno/ROOMS/anal_alley2.c",
           ]));

}
