// -*- LPC -*- //
// Coded by the local idiot Kyricc //

#include <mudlib.h>
inherit ROOM;

object storeroom;                    // The storage room for this shop

void
init()
{

  ::init();

  add_action("list", "list");
  add_action("buy", "buy");
  add_action("sell", "sell");
  add_action("value", "value");

}

void
create()
{

  object weap;

  ::create();

  storeroom = new(ROOM);                         // _clone_ a new blank room

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
  set_exits((["west"    : "/room/storage",
	      "backroom" : storeroom,                // Exit to the storeroom
	      "south" : "vill_road2",
	      ]));

}

int
list()
{

  object ob;

  int *money;

  foreach(ob in all_inventory(storeroom))
  {
    money = ob->query_value();

    if(ob->short())
      write(ob->short() + "   :  " +
	    (money[0]               ? money[0] + " silver" : "") +
	    ((money[0] && money[1]) ? " and, " : "") +
	    (money[1]               ? money[1] + " gold.\n" : ".\n"));
  }

  return 1;
}

int
buy(string item)
{
  object ob, buyer;
  int *money;
  buyer = this_player();

  if(!item)
    notify_fail("Buy what?\n");

  ob = present(item, storeroom);
  if(!ob)
    return notify_fail("That is not for sale!\n");

  money = ob->query_value();

  if(!buyer->remove_money(money))
    return notify_fail("You don't have enough money!\n");

  ob->move(this_player());

  write("You buy " + ob->short() + ".\n");
  tell_room(this_object(),
	    buyer->short() + " buys " + ob->short() + ".\n", buyer);

  return 1;
}

int
sell (string item)
{
  object ob, seller;
  int *money;
  seller = this_player();

  if(!item)
    notify_fail("Sell what?\n");

  ob = present(item, seller);

  if(!ob)
    return notify_fail("You do not have that to sell!\n");

//*  Need to figure out here what % of the value we are going to give players
//*  Also how ti figure the %age, and at what worth we will not sell an item
//*  back.

  if(!ob->drop())
    return notify_fail("You can get rid of that!\n");

  money = ob->query_value();

  seller->add_money(money);

  ob->move(storeroom);

  write("You sell " + ob->short() + ".\n");
  tell_room(this_object(),
	    seller->short() + " sells " + ob->short() + ".\n", seller);
  return 1;
}

int
value (string item)
{
  object ob, valuer;

  int *money;
  valuer = this_player();

  if(!item)
    notify_fail("Value what?\n");

  ob = present(item, valuer);

  if(!ob)
    return notify_fail("You don't have that item!\n");

  money = ob->query_value();

  if(ob->short()) write(ob->short() + "   : " +
			(money[0] ? money[0] + " silver" : "") +
			(money[0] && money[1] ? " and, " : "") +
			(money[1] ? money[1] + " gold.\n" : ".\n"));
  return 1;
}


