/*  -*- LPC -*-  */
// -= Coded by the local idiot Kyricc=-

#include <mudlib.h>

inherit ROOM;

void
create()
{
  set_light(1);
  set_short();
  set_long();
  set_exits((["" : "",
	      ]));
}
