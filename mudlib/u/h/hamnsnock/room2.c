#include <mudlib.h>

inherit ROOM;

string room_desc = "You this a test room... \n  remember this is only a test room.\n";
void create()
{
   ::create();
   set_light(1);
    set_short("room");
     set_long(room_desc);

}
