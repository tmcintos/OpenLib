// -*- LPC -*- //
// Coded by the local idiot Kyricc //

#include <mudlib.h>
inherit ROOM;

object storeroom;                           // The storage room for this shop

void
create()
{

  ::create();

  storeroom = new(ROOM);                         // _clone_ a new blank room
  storeroom->set_light(1);
  storeroom->set_short("The Storeroom");
  storeroom->set_long("This is where the items in the shop's"
		      "inventory are kept...players KEEP OUT!\n");
  storeroom->set_exits((["shop" : this_object()]));

// Now an object 'ob' can be moved into storeroom by doing ob->move(storeroom);
// and be found in the storeroom by doing ob2 = present("whatever", storeroom);

  set_light(1);
  set_short("The Generic Shop.\n");
  set_long("This is the generic shop.\n"+
	   "Here you can buy items, sell items, list the items for sale,\n"+
	   "and check the value of an item.\n");
  set_exits((["start"    : "/d/base/start.c",
	      "backroom" : storeroom,                // Exit to the storeroom
	      ]));

}
