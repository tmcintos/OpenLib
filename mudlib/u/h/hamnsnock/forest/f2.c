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
 set_exits((["north" : "/u/h/hamnsnock/forest/f3.c",
             "east" : "/u/h/hamnsnock/forest/f9.c",
             "south" : "/u/h/hamnsnock/forest/f1.c",
              "west" : "/u/h/hamnsnock/forest/f11.c"
             ]));
}
