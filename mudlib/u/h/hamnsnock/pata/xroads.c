#include <mudlib.h>

inherit ROOM;

object ob;
void create()
{
     ::create();
     set_light(1);
     set_short("Cross Roads of Patagonia");
     set_long("You stand at the CrossRoads of Patagonia.  The road here reaches all four ways.  North, South, East and West.  This is a very busy road.  There are people running in and out.  You can hear many conversations as people pass you.  The road you stand on is made out of little tiny stones, which are perfectly round, as if they were made of magic.  They don't seem to hurt your feet, in fact, they are very comforatable.\n");
     set_exits(([ "east" : "/u/h/hamnsnock/pata/indoe1.c"
              , "west" : "/u/h/hamnsnock/pata/indow1.c"
                  , "south" : "/u/h/hamnsnock/pata/timbers1.c"
              ]));

     ob=clone_object("/u/h/hamnsnock/stuff/shanu.c");
     ob->move(this_object());
}
