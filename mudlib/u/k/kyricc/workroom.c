/*  -*- LPC -*-  */
// -= Coded by the local idiot Kyricc=-

#include <mudlib.h>

inherit ROOM;

void
create()
{
  ::create();
  set_light(1);
  set_short("This is Kyricc's laboratory.");
  set_long("This is Kyricc's laboratory.  Here you see shelves on\n"+
         "three of four walls.  Two of the walls contain a myriad\n"+
         "of acient tomes.  The amount of arcane information in\n"+
         "this one room is overwhelming.  On the third wall there\n"+
         "are quite a few specimen jars containing various spell\n"+
         "components.  On the fourth wall you see a large mirror.\n"+
         "There is a table in the middle of the room, on which you\n"+
         "see a large volume of forgotten lore, a quill pen, and a\n"+
         "one foot long piece of tree branch.  This place gives\n"+
         "you the creeps.\n");
  set_exits((["start" : "/d/base/start.c",
              "crys" : "/u/k/kyricc/Crysanthium/town_square.c",
              "void" : "/adm/obj/daemon/void.c",
              "porno" : "/u/k/kyricc/Porno/porno",
	      ]));
}
