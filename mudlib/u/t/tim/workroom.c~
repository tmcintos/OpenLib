/*  -*- LPC -*-  */
#include <mudlib.h>

inherit ROOM;

void
create()
{
  ::create();
  set_light(1);
  set_short("This is the generic Wizard's workroom.\n");
  set_long("This is where, as a wizard, you will do the enormous\n"+
         "ammounts of coding you will no doubt be doing.  Feel free\n"+
         "to alter it any way you like.\n");
  set_items((["book" : "It is a boring book on magnetostatics.\n"]));
  set_exits((["start" : "/d/base/start.c",
              "void" : "/adm/obj/daemon/void.c",
	      ]));
  replace_program(ROOM);
}
