#include <mudlib.h>

inherit ROOM;

void create()
{
     object guard;
     ::create();
     set_light(1);
     set_short("Village Gates East");
     set_long("You stand at the village gates of Patagonia.  They gates are golden, and are very bright when sunlight hits them.  There is a village guard at the right side of the gate greeting people coming and going out of the village.  You stand on nice green grass thats just a little wet.\n");
     set_exits(([ "west" : "/u/h/hamnsnock/pata/indoe2.c",
                  "east" : "/u/h/hamnsnock/forest/f1.c"
              , "north" : "/u/h/hamnsnock/forest/f11.c",
                "south" : "/u/h/hamnsnock/forest/f12.c"
              ]));
   guard=new("/u/h/hamnsnock/stuff/vguard.c");
   guard->move(this_object());
}
