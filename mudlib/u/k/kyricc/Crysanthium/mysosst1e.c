/*  -*- LPC -*-  */
// -= Coded by the local idiot Kyricc=-

#include <mudlib.h>

inherit ROOM;

void
create()
{

  ::create();

  set_light(1);
  set_short("You are on Mysos St. east of the toen square.\n");
  set_long("You are on Mysos St. east of the town square.  To the\n"+
         "north is a shop, west is the town square, and the street\n"+
         "continues to the east.\n");
  set_exits((["north" : "/u/k/kyricc/Crysanthium/mysosst_shop.c",
              "east" : "/u/k/kyricc/Crysanthium/mysosst2e.c",
	      "west" : "/u/k/kyricc/Crysanthium/town_square.c",
	      ]));
}
