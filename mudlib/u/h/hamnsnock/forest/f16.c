#include <mudlib.h>

inherit ROOM;

void create()
{
     ::create();
     set_light(1);
     set_short("Forest");
     set_long("You stand in a forest.  There are tall trees that reach high into the sky.  There are large rocks at the base of one of the trees.\n");
    set_exits(([ "south" : "/u/h/hamnsnock/forest/f18.c",
                 "east"  : "/u/h/hamnsnock/forest/f15.c",
                 "west" : "/u/h/hamnsnock/pata/vgatess.c"
              ]));
}
