/*  -*- LPC -*-  */
// -======<--* "Is this gonna work?" said Warthog *-->======-

#include <mudlib.h>

inherit ROOM;

void
create()
{

  ::create();

  set_light(1);
  set_short ("West International Speedway Boulevard (2)");
  set_long (@DONE
Still in western Daytona Beach. To the north is a Perkins. To the 
south is the Daytona Beach International Speedway. 
DONE);

  set_exits((["west" : "/u/w/warthog/Daytona/volusia1.c",
      	      "north" : "/u/w/warthog/Daytona/perkins.c",
	      "south" : "/u/w/warthog/Daytona/speedway1.c",
	      "east" : "/u/w/warthog/Daytona/volusia3.c",       
               ])); 
}
