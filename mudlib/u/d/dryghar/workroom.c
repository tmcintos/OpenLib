/*  -*- LPC -*-  */
// -= Coded by the local idiot Kyricc=-
// And then Dryghar got to it:P
#include <mudlib.h>

inherit ROOM;

void
create()
{
  ::create();
  set_light(1);
set_short("this room is the std workroom edit it's features to your liking.\n");
set_long("This is the standard workroom for the wizards on Dysfunctional\n+ O"+
"mud, You must edit it's description and exits etc.. To your liking\n"+
"feel free to do whatever you want to it.. Just don't break it:P\n");
  set_exits((["start" : "/d/base/start.c",
		"void" : "/adm/obj/daemon/void.c",
// You may want to set an exit for your area or test rooms
	      ]));
}
