#include <mudlib.h>

inherit ROOM;

void create()
{
     ::create();
     set_light(1);
     set_short("forest");
     set_long("You stand in a dense forest, with lots of trees everywhere.  You see a rat run by your foot when you look down.  There are birds nested in the trees high above you.\n");
     set_exits(([ "north" : "/u/h/hamnsnock/forest/f1.c",
                  "south" : "/u/h/hamnsnock/forest/f20.c",
                 "east"  : "/u/h/hamnsnock/forest/f14.c",
                  "west"  : "/u/h/hamnsnock/forest/f12.c"
               ]));
}
