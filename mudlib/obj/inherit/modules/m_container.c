//  -*- LPC -*-
// m_container.c:  Functions for containers
//
// Copyright (C) 1996 Tim McIntosh (tmcintos@dm.imaginary.com)
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
// Orig by Tim 23 Apr 96
//

#include "m_container.h"

//
// Global Variables
//
private static int weight_capacity;
private static int bulk_capacity;
private static int rigid;            // rigid container or not?

void
initialize()
{
  set_object_class(query_object_class() | OBJECT_CONTAINER);
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
  string sh = capitalize(short());

  if( bulk_capacity && 
      ob->query_bulk() + query_bulk_contained() > bulk_capacity )
    return notify_fail(sh + " will not hold that large of object!\n");

  if( weight_capacity &&
      ob->query_weight() + query_weight_contained() > weight_capacity )
    return notify_fail(sh + " cannot hold that much weight!\n");

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

//
// override of ::long()
//
string
long()
{
  object* obs = all_inventory(this_object());
  string ret;

  if( living(this_object()) )
    ret = sprintf("  %s is carrying ",  capitalize(pronoun(this_object(), 0)));
  else
    ret = "  It contains ";
  
  if( sizeof(obs) )
    ret += list_obs(obs) + ".\n";
  else
    ret += "nothing.\n";

  return ret;
}
