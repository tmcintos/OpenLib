/*  -*- LPC -*-  */
#include <mudlib.h>

inherit ROOM;

void
create()
{
  ::create();
  set_light(1);
  set_short("Just off the center of the MUD\n");
  set_long("You are just east of the CENTER of the MUD.\n");
  set_exits((["west" : "/d/base/start",
	      ]));
  replace_program(ROOM);
}
