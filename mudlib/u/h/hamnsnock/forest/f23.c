#include <mudlib.h>

inherit ROOM;

void create()
{
     ::create();
     set_light(1);
     set_short("forest");
     set_long("You stand in a foret.  Tall trees surround you, along with large green bushes.  There are also large rocks around the ground.\n");
     set_exits(([ "north" : "/u/h/hamnsnock/forest/f22.c",
                  "west" : "/u/h/hamnsnock/forest/f21.c"
              ]));
}
