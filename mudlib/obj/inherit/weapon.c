/*  -*- LPC -*-  */
// Basic weapon Inheritable.
// Orig by Casper 9/95
// 10/14/95   Tim: removed set_long/short to work with changes to OBJECT,
//                 and removed var's long and short
// 04/20/96   Tim: updated w.r.t. object.c
// 04/23/96   Modularized by Tim
 
#include <mudlib.h>
#include <modules.h>

inherit OBJECT;
inherit M_WEAPON;

//
// Private Functions
//
void unwield();

void
initialize()
{
  object::initialize();
  m_weapon::initialize();
}

varargs string short(int fflag)
{
  if( fflag )
    return short_desc;
  return short_desc + (query_wielded() ? "(wielded)" : "");
}

int move(mixed dest)
{
  unwield();
  return object::move(dest);
}

int remove()
{
  unwield();
  return object::remove();
}

private void
unwield()
{
  if( query_wielded() )
    environment()->do_unwield(this_object(), 1);
}
