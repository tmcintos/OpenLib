#include <mudlib.h>

inherit ROOM;

void create()
{
     ::create();
     set_light(1);
     set_short("Indo Road east 2");
     set_long(@endlong
You stand on Indo Road.
  You get a very good feeling from this part of the road, probably because just
north of here is the village church.  There are birds flying above you singing
their birdy songs.  Outside the church there are flowers in pots on both sides
of the large double doors that lead inside.  There is a holy cross above the
door, and the windows you can see are filled with color and life.
endlong);
     set_exits(([ "west" : "/u/h/hamnsnock/pata/indoe1.c",
                  "north" : "/u/h/hamnsnock/pata/church.c"
              , "east" : "/u/h/hamnsnock/pata/vgatese.c"
               ]));
}
