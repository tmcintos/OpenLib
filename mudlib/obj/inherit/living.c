/*  -*- LPC -*-  */
// living.c:  Tim:  10/03/95
// Modularized 24 Apr 96

#pragma save_binary
#pragma no_warnings
#include <mudlib.h>
#include <modules.h>

inherit BASE;

inherit M_CLEAN_UP;
inherit M_OBJECT_CLASS;
inherit M_FLAGS;
inherit M_VISIBLE;
inherit M_MOVE;
inherit M_DIMENSIONS;
inherit M_CONTAINER;
inherit M_MONEY_CONTAINER;
inherit M_COMBAT;
inherit M_LIVING;
inherit MODULE_DIR "/m_race";

void
initialize()
{
  m_flags::initialize();
  m_visible::initialize();
//  m_combat::initialize();
  m_money_container::initialize();
}

void
low_setup()
{
//  m_combat::low_setup();
  m_living::low_setup();
}

void
remove()
{
  m_living::remove();
}

int
id(string arg)
{
  return (m_living::id(arg) || m_visible::id(arg));
}

string
long()
{
  return short() + ".\n" + m_visible::long() +
    m_race::long() + m_living::long() +  m_container::long();
}
