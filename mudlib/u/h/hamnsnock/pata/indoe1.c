#include <mudlib.h>

inherit ROOM;

void create()
{
     ::create();
     set_light(1);
     set_short("indo east 1");
     set_long("You stand on Indo Road.  It reaches all the way east, and back all the way west.  As you walk around the village, you get a very good feeling.  Its not that windy out, and there are a lot of green trees, bushes, and plants all around the place to keep it looking alive.\n");
     set_exits(([ "west" : "/u/h/hamnsnock/pata/xroads.c"
               , "east" : "/u/h/hamnsnock/pata/indoe2.c"
              ]));
}
