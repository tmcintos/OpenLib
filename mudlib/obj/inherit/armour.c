/*  -*- LPC -*-  */
// Basic armour Inheritable.
// Orig by Casper 9/95
// 10/14/95   Tim: removed set_long/short to work with changes to OBJECT,
//                 and removed var's long and short
// 04/20/96   Tim: updated w.r.t. object.c
// 04/23/96   Modularized by Tim.

#include <mudlib.h>
#include <modules.h>

inherit OBJECT;
inherit M_ARMOUR;

//
// Private Functions
//
void unwear();

void initialize()
{
  object::initialize();
  m_armour::initialize();
}
 
string short(int fflag)
{
  if( fflag )
    return short_desc;
  else
  {
    string where = query_worn();
    return short_desc + (where ? "("+where+")" : "");
  }
}

int move(mixed dest)
{
  unwear();
  return object::move(dest);
}

int remove()
{
  unwear();
  return object::remove();
}

private void
unwear()
{
  if( query_worn() )
    environment()->do_remove(this_object(), 1);
}
