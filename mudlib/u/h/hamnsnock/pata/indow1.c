#include <mudlib.h>

inherit ROOM;

void create()
{
       ::create();
     set_light(1);
     set_short("Indo Road West 1");
     set_long("You stand on Indo road.  It reaches both west and east.  Their are birds flying above you as you walk along.  There are bright green bushes at both sides of the road.\n");
     set_exits(([ "east" : "/u/h/hamnsnock/pata/xroads.c"
                 , "west" : "/u/h/hamnsnock/pata/indow2.c"
               ]));
}
