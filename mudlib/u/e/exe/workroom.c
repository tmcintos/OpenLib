#include <mudlib.h>

inherit ROOM;

void
create()
{
  ::create();
  set_light(1);
  set_short("Exe's workroom.\n");
  set_long(@LONGTEXT
You stand in a room full of NOTHING. Most rooms have stuff in them but
this room has nothing whatsoever. Not a sausage. Zip. Goose eggs.
Abosute zero. Totally nothing.
Apart from the floor, a few doors and the walls there is nothing in the
room. So amazed by all of this you decide to do somthing very unlike you.
You take off your shoes and put them on the coffee table, another fixture
that isn't here, and lie down on the large bed, which also isn't here in
the nothing.
LONGTEXT);

  set_exits((["start" : "/d/base/start.c",
              "void" : "/adm/obj/daemon/void.c",
              ]));
}
