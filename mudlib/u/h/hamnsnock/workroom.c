#include <mudlib.h>

inherit ROOM;

void
initialize()
{
  ::initialize();
  set_light(1);
  set_short("This is Hamnsnock's workroom.\n");
  set_long("You look around the room and can just feel the evil.  there\n"+
         "are giant spell books everywhere.  And a lot of dark red evil\n"+
         "potions to choose from.  There is even a book on the desk, \n"
         "you can see some writing on it.\n");
  set_items((["book" : "It is a boring book on evil.\n"]));
  set_exits((["start" : "/d/base/start.c",
              "church" : "/room/church.c",
              "void" : "/adm/obj/daemon/void.c",
              ]));
}
