#include <mudlib.h>

inherit ROOM;

void create()
{
     ::create();
     set_light(1);
     set_short("forest");
     set_long("You stand in a forest.  There are lots of trees everywhere.  You think you can hear some people talking far in the distance.\n");
     set_exits(([ "north" : "/u/h/hamnsnock/forest/f13.c",
                  "south" : "/u/h/hamnsnock/forest/f21.c",
                  "east"  : "/u/h/hamnsnock/forest/f22.c",
                  "west"  : "/u/h/hamnsnock/forest/f15.c"
               ]));
}
