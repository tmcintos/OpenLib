#include <mudlib.h>

inherit ROOM;

void create()
{
object rat;
 ::create();
 set_light(1);
 set_short("Light Forest");
 set_long("You stand in a dense forest.  It has trees all around you.\n"+
          "As you look around, its hard to see because of all the trees\n"+
          "and bushes.\n");
 set_exits(([
             "east" : "/u/h/hamnsnock/forest/f6.c",
             "south" : "/u/h/hamnsnock/forest/f4.c",
             ]));
   rat = new("/u/h/hamnsnock/stuff/rat.c");
   rat->move(this_object());
}
