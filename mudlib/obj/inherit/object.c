/*  -*- LPC -*-  */
// object.c:  a representation of a normal object
// Tim 23 Apr 96

#pragma save_binary
#pragma no_warnings

#include <mudlib.h>
#include <modules.h>
#include <object_types.h>

inherit BASE;

inherit M_CLEAN_UP;
inherit M_OBJECT_CLASS;
inherit M_FLAGS;
inherit M_VISIBLE;
inherit M_MOVE;
inherit M_VALUE;
inherit M_DIMENSIONS;

void
initialize()
{
  m_flags::initialize();
  m_visible::initialize();
  m_value::initialize();

  set_object_class(OBJECT_OBJECT | query_object_class());
}
