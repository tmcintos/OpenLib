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
  return object::long() + m_container::long();
}
