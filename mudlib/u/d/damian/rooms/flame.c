/*  -*- LPC -*-  */

#include <mudlib.h>

inherit ROOM;

void
create()
{
  ::create();
  set_light(1);
  set_short("The residence of Damian");
  set_long("You have entered the residence of Damian the Shadowlord.\n"+
           "Swaths of darkness float about the room and coallesce\n"+
           "about your shoulders.  The room is otherwise barren.\n");
  set_exits((["start" : "/d/base/start.c",
              "void" : "/adm/obj/daemon/void.c",
	      ]));
}
