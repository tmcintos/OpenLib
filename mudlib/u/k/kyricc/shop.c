// -*- LPC -*- //
// Coded by the local idiot Kyricc //

#include <mudlib.h>
inherit ROOM;

object storeroom, weap;                    // The storage room for this shop

void
init()
{

  ::init();

  add_action("list", "list");
  add_action("buy", "buy");

}

void
create()
{

  ::create();

  storeroom = new(ROOM);                         // _clone_ a new blank room
  storeroom->clone("/obj/examples/weapon.c");
  storeroom->set_light(1);
  storeroom->set_short("The Storeroom");
  storeroom->set_long("This is where the items in the shop's "
		      "inventory are kept...players KEEP OUT!\n");
  storeroom->set_exits((["shop" : this_object()]));

// Now an object 'ob' can be moved into storeroom by doing ob->move(storeroom);
// and be found in the storeroom by doing ob2 = present("whatever", storeroom);

  weap = new("/u/k/kyricc/WEAPONS/kdofa.c");
  weap->move(storeroom);
  weap = new("/u/k/kyricc/ARMOURS/kcod.c");
  weap->move(storeroom);
  
  set_light(1);
  set_short("The Generic Shop.\n");
  set_long("This is the generic shop.\n"+
	   "Here you can buy items, sell items, list the items for sale,\n"+
	   "and check the value of an item.\n");
  set_exits((["start"    : "/d/base/start.c",
	      "backroom" : storeroom,                // Exit to the storeroom
	      ]));

}

int
list()
{

  object ob;
  string *money;

  foreach(ob in all_inventory(storeroom))
  {
    money = ob->query_value();
  if(ob->short()) write(ob->short() + "   :  " + (money[0] ? money[0]
    + " silver" : "")+ (money[0] && money[1] ? "and, " : "") + (money[1]
    ? money[1] + " gold.\n" : ".\n"));
  }
  return 1;

}

int
buy(string item)
{
  object ob, buyer;
  int *money;
  buyer = this_player();

  storeroom->query_inventory(item);
    if(!item) notify_fail("That is not for sale!\n");
  ob = present(item, storeroom);

  money = ob->query_value();
    if(!buyer->remove_money(money)) notify_fail("You don't have enough money!\n");
      ob->move(this_player());
      write("You buy " + ob->short() + ".\n");
      tell_room(this_object(), buyer->short() + " buys " + ob->short() + ".\n", buyer);
  return 1;
}
