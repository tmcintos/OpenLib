#include <mudlib.h>

inherit ROOM;

void create()
{
     ::create();
     set_light(1);
     set_short("forest");
     set_long("You stand in a forest.  The talls are very tall and surround you.  There are birds flying high above you in the sky.  The air is nice and cool.\n");
     set_exits(([ "north" : "/u/h/hamnsnock/forest/f16.c",
                  "west" : "/u/h/hamnsnock/forest/f17.c",
                  "east" : "/u/h/hamnsnock/forest/f19.c"
               ]));
}
