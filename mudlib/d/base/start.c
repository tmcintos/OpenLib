/*  -*- LPC -*-  */
#include <mudlib.h>

inherit ROOM;

void
initialize()
{
  set_light(1);
  set_short("Center of the MUD\n");
  set_long(@end
You are at the exact center of the MUD.  This appears to
be as far as the known universe extends.
end);
  set_exits((["east" : "/d/base/start2",
	      "west" : "/d/base/start3",
	      ]));
  replace_program(ROOM);
}
