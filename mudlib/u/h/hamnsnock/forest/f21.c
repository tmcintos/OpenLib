#include <mudlib.h>

inherit ROOM;

void create()
{
     ::create();
     set_light(1);
     set_short("forest");
     set_long("You stand in a dense part of the forest.  There are trees and bushes everywhere you look.  You just saw a rat pass by your foot.\n");
     set_exits(([ "north" : "/u/h/hamnsnock/forest/f20.c",
                  "west" : "/u/h/hamnsnock/forest/f19.c",
                  "east" : "/u/h/hamnsnock/forest/f23.c"
               ]));
}
