#include <mudlib.h>

inherit ROOM;

void create()
{
     ::create();
     set_light(1);
     set_short("Forest");
     set_long("You stand in a dense forest with lots of green trees.  You can see some green moss on the side of one of the tress.\n");
     set_exits(([ "north" : "/u/h/hamnsnock/forest/f12.c",
                  "south" : "/u/h/hamnsnock/forest/f19.c",
                  "east"  : "/u/h/hamnsnock/forest/f20.c",
                  "west"  : "/u/h/hamnsnock/forest/f16.c"
               ]));
}
