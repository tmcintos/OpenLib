#include <mudlib.h>

inherit ROOM;

void create()
{
     ::create();
     set_light(1);
     set_short("Timberland Road south #3");
     set_long(@endlong
You stand on the long road of Timberland.  This road reaches towards the north and towards the south.  The famous Xantie square lies towards the east of you.
endlong);
      set_exits(([ "south" : "/u/h/hamnsnock/pata/vgatess.c",
                   "north" : "/u/h/hamnsnock/pata/timbers2.c",
                   "east" : "/u/h/hamnsnock/pata/xantie3.c"
              ]));
}
