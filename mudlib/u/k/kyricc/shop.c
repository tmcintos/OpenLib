// -*- LPC -*- //
// Coded by the local idiot Kyricc //

include <mudlib.h>
inherit ROOM;

object storeroom;

void
init()
{

  ::init();
  add_action("buy", "buy");
  add_action("list", "list");
  add_action("sell", "sell");
  add_action("value", "value");

}

void
create()
{

  ::create();

  storeroom = new(ROOM);
  storeroom->set_light(1);
  storeroom->set_short("The Storeroom.\n");
  storeroom->set_long("This is the shop storeroom where all of the items that\n"+
		      "that are for sale are kept.\n");
  storeroom->set_exits("shop" : this_object());

  set_light(1);
  set_short("The Generic Shop.\n");
  set_long("This is the generic shop.\n"+
	   "Here you can buy items, sell items, list the items for sale,\n"+
	   "and check the value of an item.\n");
  set_exits((["start" : "/d/base/start.c",
	      ]));

}

buy(item)
{

  if(!item)
    return 0;
  call_other("room/store", "but", item);
  return 1;

}

list()
{
}

sell(item)
{

  object ob;

  if(!item)
    return 0;
  if(item == "all") {
    object next;
    ob = first_inventory(this_player());
    while(ob) {
      next = next_inventory(ob);
      if(!ob)->drop() && ob->query_value()) {
	write(ob)->short() + ":\t");
        do_sell(ob); }
      ob = next;
    }
    write("Ok.\n");
    return 1;
    }
    
    ob = present(item, this_player());
    if(!ob)
     ob = present(item, this_object());
    if(!ob) {
        write("No such item ("); write(item); write(") here.\n");
	return 1;
      }

    do_sell(ob);
    return 1;

}

value(item)
{

  int value;
  string name_of_item;

  if(!item)
    return 0;

  name_of_item = present(item);
  if(!name_of_item)
    name_of_item = find_item_in_player(item);
  if(!name_of_item) {
    if(call_other("room/store", "value", item))
      return 1;
    write("No such item ("); write(item); write(") here\n");
    write("or in the store.\n");
    return 1;
  }
  write("You would get "); write(value); write(" gold coins.\n");
  return 1;

}
