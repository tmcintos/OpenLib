/*  -*- LPC -*-  */
// -======<--* "Is this gonna work?" said Warthog *-->======-

#include <mudlib.h>

inherit ROOM;

void
create()
{

  ::create();

  set_light(1);
  set_short ("Dog Track");
  set_long (@DONE
You have entered the Daytona Beach Kennel Club.  Here you can 'bet' on 
the doggies.  (This room's not complete)
DONE);

  set_exits(([
      	      "north" : "/u/w/warthog/Daytona/volusia1.c",
               ])); 
}
