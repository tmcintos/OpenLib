#include <mudlib.h>

inherit ROOM;

void create()
{
     ::create();
     set_light(1);
     set_short("Indo Rd. West 2");
     set_long(@endlong
You stand on Indo Road.  Green bushes are on both sides of the road.  There
are many people passing you heading east and west.  The town shop is just 
north of you and is always open.  Learners Way is South of you, and the rest of the road reaches both east and west.
endlong);
     set_exits(([ "east" : "/u/h/hamnsnock/pata/indow1.c",
                  "north" : "/u/h/hamnsnock/pata/vshop.c"
               ]));
}
