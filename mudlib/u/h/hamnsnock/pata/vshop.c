#include <mudlib.h>

inherit ROOM;

void create()
{
     ::create();
     set_light(1);
     set_short("Village Shop");
     set_long("You stand in the large village shop.  There are many many weapons and other good things along the walls.  They all have pricetags on them.\n **SHOP is underconstruction, but hey.. what isn't?\n");
     set_exits(([ "out" : "/u/h/hamnsnock/pata/indow2.c"
              ]));
}
