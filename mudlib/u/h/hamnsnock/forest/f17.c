#include <mudlib.h>

inherit ROOM;

void create()
{
     ::create();
     set_light(1);
     set_short("forest");
     set_long("You stand in a large forest.  From the north you can hear a lot of people talking.  There is a grass pathway that leads you to the north also.  There are tall tress all around you.\n");
     set_exits(([ "north" : "/u/h/hamnsnock/pata/vgatess.c",
                  "east" : "/u/h/hamnsnock/forest/f18.c"
               ]));
}
