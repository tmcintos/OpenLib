/*  -*- LPC -*-  */
#include <mudlib.h>

inherit ROOM;

void
create()
{
  ::create();
  set_light(1);
  set_short("Center of the MUD\n");
  set_long(@end
The void.
end);
}
