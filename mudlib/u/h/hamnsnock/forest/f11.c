#include <mudlib.h>

inherit ROOM;

void create()
{
     ::create();
     set_light(1);
     set_short("forest");
     set_long("You stand in a light forest.  All kinds of trees are around you.  You can hear the busy village of Patagonia to the south of you.\n");
     set_exits(([ "south" : "/u/h/hamnsnock/pata/vgatese.c"
                  , "east" : "/u/h/hamnsnock/forest/f2.c"
               ]));
}
