/*  -*- LPC -*-  */
// file: money.c
// created by Tim on: 10/14/95
// purpose: clonable and 'get'able money object.  Contains a set amount of
//          money, and adds this amount to the user's pocket change when
//          picked up.  Also adds the coins' weight to the user & destructs
//          when picked up.

#include <mudlib.h>
#include <object_types.h>
#include <money.h>

inherit OBJECT;

private static int *coins;                   /* coins this object contains */

void
create()
{
  coins = allocate(COIN_TYPES);
  set_short("a pile of money");
  set_ids(({"money", "coins", "pile"}));
}

int
set_money(int *money)
{
  int i, weight = 0;

  if(sizeof(money) < COIN_TYPES)
    return 0;

  for(i = 0; i < COIN_TYPES; i++) {
    coins[i] = money[i];
  }

  weight += coins[SILVER] / 18;
  weight += coins[GOLD] / 6;
  set_weight(weight);

  return 1;
}

int *
query_money()
{
  return coins;
}

varargs
mixed
long(int flag)
{
  string ret = "";

  ret += sprintf("%s containing %d silver coins and %d gold coins.\n",
		 capitalize(short_desc),
		 coins[SILVER], coins[GOLD]);

  if(flag)
    return ret;

  write(ret);
}

int
move(mixed dest)
{
// If we move to a living object, we increment their money supply & destruct
  if(object::move(dest)) {
    if(dest->query_object_class() & OBJECT_LIVING) {
      dest->add_money(coins);
      remove();
      return 1;
    } else {
      return 1;
    }
  }
  return 0;
}
