/*  -*- LPC -*-  */
// -======<--* "Is this gonna work?" said Warthog *-->======-

#include <mudlib.h>

inherit ROOM;

void
create()
{

  ::create();

  set_light(1);
  set_short ("Pizza Hut");
  set_long (@DONE
This is the 'Speedway Pizza Hut'.  It is not as good as the PH in Port 
Orange and not even close to the downtown Lincoln (Nebraska) Pizza Hut,
but for your purposes, it will work.  Type 'list' to see the items you
can order.  (If you can read this, this room is not finished) 
DONE);

  set_exits(([
	      "south" : "/u/w/warthog/Daytona/volusia1.c",
               ])); 
}
