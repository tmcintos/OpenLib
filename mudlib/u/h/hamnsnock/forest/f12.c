#include <mudlib.h>

inherit ROOM;

void create()
{
     ::create();
     set_light(1);
     set_short("forest");
     set_long("You stand in a forest that has lots of different kinds of trees all around you.  There are birds flying in the sky directly above you.  You can hear the busy village of Patagonia north of you.\n");
     set_exits(([ "north" : "/u/h/hamnsnock/pata/vgatese.c",
                  "east" : "/u/h/hamnsnock/forest/f13.c",
                  "south" : "/u/h/hamnsnock/forest/f15.c"
               ]));
}
