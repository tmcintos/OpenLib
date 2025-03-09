/*  -*- LPC -*-  */
#include <mudlib.h>

inherit ROOM;

void
create()
{
  ::create();
  set_light(1);
  set_short("Center of the MUD\n");
  set_long("You are at the exact center of the MUD.  This appears to\n"+
	"be as far as the known universe extends.\n");
  set_exits((["east" : "/d/base/start2",
	      "west" : "/d/base/start3",
	      ]));
}
