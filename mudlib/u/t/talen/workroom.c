/*  -*- LPC -*-  */
// -= Coded by the local idiot Kyricc=-

#include <mudlib.h>

inherit ROOM;

void
create()
{
  ::create();
  set_light(1);
  set_short("Talen's workroom.\n");
  set_long("You wander in - and stare, somewhat amazed, at all the mess.\n" +
	"Books are scattered all over the floor; papers are strewn everywhere,\n" +
	"you can't make head nor tail of it. Yup, Talen the Messy has been\n" +
	"here, making it look like home...\n");
  set_exits((["start" : "/d/base/start.c",
              "void" : "/adm/obj/daemon/void.c",
	      ]));
}
