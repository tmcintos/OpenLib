/*  -*- LPC -*-  */
// container.c:  An inheritable container object
// Tim 23 Apr 96

#pragma save_binary
#include <mudlib.h>
#include <modules.h>

inherit OBJECT;
inherit M_CONTAINER;

void
initialize()
{
  object::initialize();
  m_container::initialize();
}

//
// override of object::long()
//
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
