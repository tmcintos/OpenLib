#include <mudlib.h>

inherit ROOM;

void create()
{
     ::create();
     set_light(1);
     set_short("forest");
     set_long("You stand in a forest, with lots of trees and green bushes.  You see an owl looking down at you from a high tree.  The wind is blowig very cool air in your direction.\n");
     set_exits(([ "north" : "/u/h/hamnsnock/forest/f10.c",
                  "south" : "/u/h/hamnsnock/forest/f22.c",
                  "west" : "/u/h/hamnsnock/forest/f13.c"
               ]));
}
