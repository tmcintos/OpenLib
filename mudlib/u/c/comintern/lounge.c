/*  -*- LPC -*-  */
// Comintern's second LPC code!

#include <mudlib.h>

inherit ROOM;

void
create()
{
  ::create();
  set_light(1);

  set_short("The Comintern Lounge");
  set_long("Whew!  Well worth the trip!  You are standing in the middle\n"+
         "of the communist party headquarters, and the party is raging\n"+
         "around you.  A funky disco ball is spinning at a breakneck\n"+
         "pace over the dancefloor, and the bar in back looks to be well\n"+
           "stocked with good proletarian beers like Guinness and Bass.\n"+
         "Several posh-looking sofas line the far wall, and it looks like\n"+
         "the perfect place to sit down, suck back a few brews and plan\n"+
         "on conquering the world for the workers.\n");
  set_exits((["office" : "/u/c/comintern/workroom.c",
	      ]));
}
