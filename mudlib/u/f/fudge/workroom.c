/*  -*- LPC -*-  */
// -= Coded by the local idiot Kyricc=-

#include <mudlib.h>

inherit ROOM;

void
initialize()
{
  ::initialize();     // call to the original initialize() function

  set_light(1);       // set light level in this room to 1

  set_short("This is the generic Wizard's workroom.\n");

  set_long("This is where, as a wizard, you will do the enormous "
	   "amounts of coding you will no doubt be doing.  Feel free "
	   "to alter it any way you like.\n");

  set_exits((["start" : "/d/base/start.c",
              "void" : "/adm/obj/daemon/void.c",
	      ]));
}
