#include <mudlib.h>

inherit ROOM;

void create()
{
     ::create();
     set_light(1);
     set_short("Timberland Road South #2");
     set_long(@endlong
You stand on Timberland Road.  The road reaches South and North, and to the
east is the famous Xantie square.  Towards the west is Learners way, where
the famous School of magic is located, along with the Library of Time.
endlong);
     set_exits(([ "north" : "/u/h/hamnsnock/pata/timbers1.c",
                  "east" : "/u/h/hamnsnock/pata/xantie1.c",
                  "south" : "/u/h/hamnsnock/pata/timbers3.c"
                    , "west" : "/u/h/hamnsnock/pata/learn4.c"
               ]));
}
