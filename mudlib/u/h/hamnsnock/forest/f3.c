#include <mudlib.h>

inherit ROOM;

void create()
{
 ::create();
 set_light(1);
 set_short("Light Forest");
 set_long("You stand in a dense forest.  It has trees all around you.\n"+
          "As you look around, its hard to see because of all the trees\n"+
          "and bushes.\n");
 set_exits((["north" : "/u/h/hamnsnock/forest/f4.c",
             "east" : "/u/h/hamnsnock/forest/f8.c",
             "south" : "/u/h/hamnsnock/forest/f2.c",
             ]));
}
