/*  -*- LPC -*-  */
// container.c:  Container object by Tim 4/20/96

#pragma save_binary

#include <mudlib.h>
#include <object_types.h>

inherit OBJECT;

private static int weight_capacity;
private static int bulk_capacity;
private static int rigid;            // rigid container or not?

void create();
string long();
nomask int set_rigid(int tf);

int query_weight_contained();
int query_bulk_contained();
int query_weight();
int query_bulk();
int receive_object(object ob);

nomask int set_weight_capacity(int lbs);
nomask int query_weight_capacity();
nomask int set_bulk_capacity(int units);
nomask int query_bulk_capacity();

/*
 * Implementation
 */
void
create()
{
  ::create();
  set_object_class(query_object_class() | OBJECT_CONTAINER);
}

string
long()
{
  string ret = ::long();
  object* obs = all_inventory(this_object());

  if( sizeof(obs) )
    ret += break_string("  It contains " + list_obs(obs) + ".\n", 70);
  else
    ret += "  It contains nothing.\n";

  return ret;
}

nomask int
set_rigid(int tf)
{
  rigid = tf;
}

int
query_weight_contained()
{
  int weight;

  foreach(object ob in all_inventory(this_object()))
    weight += ob->query_weight();

  return weight;
}

int
query_bulk_contained()
{
  int bulk;

  foreach(object ob in all_inventory(this_object()))
    bulk += ob->query_bulk();

  return bulk;
}

/*
 * override of object::query_weight()
 */
int
query_weight()
{
  return query_base_weight() + query_weight_contained();
}

/*
 * override of object::query_bulk()
 */
int
query_bulk()
{
  if( rigid )
    return query_base_bulk();
  else
    return query_base_bulk() + query_bulk_contained();
}

/*
 * override of object::receive_object()
 */
int
receive_object(object ob)
{
  if( bulk_capacity && 
      ob->query_bulk() + query_bulk_contained() > bulk_capacity )
    return notify_fail(short() + " is not large enough to hold that!\n");

  if( weight_capacity &&
      ob->query_weight() + query_weight_contained() > weight_capacity )
    return notify_fail(short() + " is not strong enough to hold that!\n");

  return 1;
}

nomask int 
set_weight_capacity(int lbs)
{
  weight_capacity = lbs;
}

nomask int
query_weight_capacity()
{
  return weight_capacity;
}

nomask int 
set_bulk_capacity(int units)
{
  bulk_capacity = units;
}

nomask int
query_bulk_capacity()
{
  return bulk_capacity;
}
