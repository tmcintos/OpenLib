/*  -*- LPC -*-  */
// -======<--* "Is this gonna work?" said Warthog *-->======-

#include <mudlib.h>

inherit ROOM;

void
create()
{

  ::create();

  set_light(1);
  set_short ("Generic Room");
  set_long (@DONE
This is a generic room to start coding from.
DONE);

  set_exits((["start" : "/d/base/start.c",
            "workroom" : "/u/w/warthog/workroom.c",
             ])); 
}
