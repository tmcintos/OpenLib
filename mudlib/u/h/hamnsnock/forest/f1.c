#include <mudlib.h>

inherit ROOM;

void create()
{
 ::create();
 set_light(1);
 set_short("Light Forest");
 set_long("You stand in a light forest.  It has hardly any trees, but you\n"+
       "can see that it gets more dense as you travel along.  You can hear the busy village of Patagonia to the west of you.\n");
 set_exits((["north" : "/u/h/hamnsnock/forest/f2.c",
             "east" : "/u/h/hamnsnock/forest/f10.c",
        "west" : "/u/h/hamnsnock/pata/vgatese.c"
          ,"south" : "/u/h/hamnsnock/forest/f13.c"
            ]));
}
