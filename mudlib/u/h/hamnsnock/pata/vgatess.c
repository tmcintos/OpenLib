#include <mudlib.h>

inherit ROOM;

void create()
{
     object guard;
     ::create();
     set_light(1);
     set_short("Village Gates East");
     set_long("You stand at the Southern Gates of Patagonia.  Here you can see all the busy people leave and enter the busy village.  You hear lots of conversations all at once.  The gates of the Village are made of gold, and are guarded by a friendly guard.  You stand on nice green cold grass, which feels very comfortble on your feet.\n");
     set_exits(([ "south" : "/u/h/hamnsnock/forest/f17.c",
                  "east" : "/u/h/hamnsnock/forest/f16.c"
                    , "north" : "/u/h/hamnsnock/pata/timbers3.c"
              ]));
   guard=new("/u/h/hamnsnock/stuff/vguard.c");
   guard->move(this_object());
}
