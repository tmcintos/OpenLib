#include <mudlib.h>

inherit ROOM;

void create()
{
     ::create();
     set_light(1);
     set_short("forest");
     set_long("You stand in a very chilly party of the forest.  You can hear the rats crawl around in the bushes.  The trees are very tall here.\n");
     set_exits(([ "north" : "/u/h/hamnsnock/forest/f14.c",
                  "south" : "/u/h/hamnsnock/forest/f23.c",
                  "west"  : "/u/h/hamnsnock/forest/f20.c"
              ]));
}
