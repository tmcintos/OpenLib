/*  -*- LPC -*-  */
// -= Coded by the local idiot Kyricc=-

#include <mudlib.h>

inherit ROOM;

void
create()
{
  set_light(1);
  set_short("You are on Lookar road, north of the square.\n");
  set_long("You are on Lookar road.  The street is made of\n"+
         "cobblestone.  There are people hurrying by you\n"+
         "going both ways.  The road continues to the north\n"+
         "and the town square is to the south.\n");
  set_exits((["north" : "/u/k/kyricc/Crysanthium/lookarvornint.c",
              "south" : "/u/k/kyricc/Crysanthium/town_square.c",
	      ]));
}
