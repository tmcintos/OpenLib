#include <mudlib.h>

inherit ROOM;

void
create()
{
  ::create();
  set_light(1);
  set_short("This is Hamnsnock's very own workroom!\n");
  set_long("     You stand in a large room with magic books everywhere.\n"+
         "There are potions of all sorts and kinds all over the room, you\n"+
         "feel very good standing in this room.  Its very magical. \n");
  set_exits((["start" : "/d/base/start.c",
               "void" : "/adm/obj/daemon/void.c"
                 ,"pata" : "/u/h/hamnsnock/pata/xroads.c"
              ]));
}
