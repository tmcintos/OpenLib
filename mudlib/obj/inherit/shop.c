// -*- LPC -*-
//
// Copyright (C) 1996 Jason Borgmann (kyricc@inetnebr.com)
//
// This program is part of the OpenLib Mudlib distribution; it
// is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published
// by the Free Software Foundation; either version 2 of the License,
// or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// If you acquired this program as part of the OpenLib Mudlib
// distribution, you should have received a copy of the GNU
// General Public License in the file /doc/GPL; if not,
// write to the Free Software Foundation, Inc., 675 Mass Ave,
// Cambridge, MA 02139, USA.
//
// Coded by the local idiot Kyricc
//
// 21 Apr 96 Tim updated with respect to new OBJECT::move() stuff
// 22 Apr 96 Tim added replace program stuff for memory savings.
//

#include <mudlib.h>

inherit ROOM;

object storeroom;                    // The storage room for this shop

void
initialize()
{

  object weap;

  ::initialize();

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
  set_exits((["start"    : "/d/base/start.c",
	      "backroom" : storeroom,                // Exit to the storeroom
	      ]));

  // This saves memory when a file inherits this one and then only defines
  // a create() function and nothing else. (see manpage)
  // This works because it doesn't replace until the current execution
  // is complete.
  if( replaceable(this_object()) )
    replace_program(SHOP);
}

void
init()
{

  ::init();

  add_action("list", "list");
  add_action("buy", "buy");
  add_action("sell", "sell");
  add_action("value", "value");

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

  if( !buyer->remove_money(money) )
    return notify_fail("You don't have enough money!\n");

  if( !(ob->move(buyer)) ) {
    buyer->add_money(money);
    return 0;  // move() sets the fail message
  }

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
    return notify_fail("You can't get rid of that!\n");

  money = ob->query_value();

  if( !(seller->add_money(money)) )
    return notify_fail("You can't carry that much money!!\n");

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


