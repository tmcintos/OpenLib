#include <mudlib.h>

inherit ROOM;

void create()
{
     ::create();
     set_light(1);
     set_short("forest");
     set_long("You stand in a large dense forest.  The trees are very tall and large.  Its hard to see behind them.  There are a lot of bushes around the base of the trees.\n");
     set_exits(([ "north" : "/u/h/hamnsnock/forest/f15.c",
                  "west" : "/u/h/hamnsnock/forest/f18.c",
                  "east" : "/u/h/hamnsnock/forest/f21.c"
                ]));
}
