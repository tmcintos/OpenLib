#include <mudlib.h>

inherit ROOM;

void create()
{
     ::create();
     set_light(1);
     set_short("Zargon's Bedroom\n");
     set_long("You stand in a very large bedroom.  There are black magic books everywhere you look.  This room is very bright, because there are large windows on three of the walls in front of you.  There is a large bed with red sheets on it directly in the center of the room.  There is a crystal ball which sits on a large stick which floats just a little bit off the ground on the right side of the bed.  On each of the four walls, there are pictures of evil magicians that Zargon looks up to.\n");
     set_exits(([ 
                 "south" : "/u/h/hamnsnock/evil/etoz.c"
               ]));
}
