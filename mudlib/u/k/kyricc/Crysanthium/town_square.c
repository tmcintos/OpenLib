/*  -*- LPC -*-  */
// -= Coded by the local idiot Kyricc=-

#include <mudlib.h>

inherit ROOM;

void
create()
{
  set_light(1);
  set_short("This is the town square of Crysanth, the capital of\n"+
         "Crysanthium.\n");
  set_long("This is the town square of Crysanth, the capital of\n"+
         "Crysanthium.  To the north and south lies Lookar road.\n"+
         "To the east and west lies Mysos street.\n\n"+
         "The empire of Crysanthium is still under construction,\n"+
         "as is it's capital.  Please disregard the mess.\n");
  set_exits((["north" : "/u/k/kyricc/Crysanthium/lookarrd1n.c",
              "south" : "/u/k/kyricc/Crysanthium/lookarrd1s.c",
              "east" : "/u/k/kyricc/Crysanthium/mysosst1e.c",
              "west" : "/u/k/kyricc/Crysanthium/mysosst1w.c",
              ]));
}
