#include <mudlib.h>

inherit ROOM;

void create()
{
 ::create();
 set_light(1);
 set_short("Light Forest");
 set_long("You stand in a light forest.  It has hardly any trees, but you\n"+
          "can see that it gets more dense as you go forwards.\n");
 set_exits((["north" : "/u/h/hamnsnock/forest/f9.c",
             "west" : "/u/h/hamnsnock/forest/f1.c",
            "south" : "/u/h/hamnsnock/forest/f14.c"
            ]));
}
