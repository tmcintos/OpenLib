/*  -*- LPC -*-  */
// -======<--* "Is this gonna work?" said Warthog *-->======-

#include <mudlib.h>

inherit ROOM;

void
create()
{

  ::create();

  set_light(1);
  set_short ("West International Speedway Boulevard");
  set_long (@DONE
You are on the west edge of Daytona Beach, "the spring break capital 
of the world!!" (ok, whatever).  To the north you see a Pizza Hut.  
To the south is the Daytona Beach Kennel Club.  East leads toward 
the beach and Clyde Morris Blvd.
DONE);

  set_exits((["start" : "/d/base/start.c",
      	      "north" : "/u/w/warthog/Daytona/pizza_hut.c",
	      "south" : "/u/w/warthog/Daytona/kennel.c",
	      "east" : "/u/w/warthog/Daytona/volusia2.c",       
               ])); 
}
