#include <mudlib.h>

inherit ROOM;

void create()
{
     ::create();
     set_light(1);
     set_short("Timberland Road South 1");
     set_long("You stand on Timberland Road.  The road goes all the way south, and back north.  Towards the south you can see a bank with many people going in and out of it. \n");
     set_exits(([ "north" : "/u/h/hamnsnock/pata/xroads.c",
                  "south" : "/u/h/hamnsnock/pata/timbers2.c"
              ]));
}
