#include <mudlib.h>

inherit ROOM;
object mon;

void create()
{
     ::create();
     set_light(1);
     set_short("Entrance to Zargon's Bedroom\n");
     set_long("You stand in a large room.  The walls are covered with "+
              "blood.  You can just feel the black magic that surrounds you.\n");
     set_exits((["north" : "/u/h/hamnsnock/evil/bedroom.c"]));
    mon = clone_object("/u/h/hamnsnock/stuff/knight.c");
   mon->move(this_object());
}
