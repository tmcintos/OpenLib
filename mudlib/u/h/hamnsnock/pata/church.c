#include <mudlib.h>

inherit ROOM;

void create()
{
     ::create();
     set_light(1);
     set_short("village church");
     set_long("You stand in a large church.  It is very lit up in here from all the candles that are lit.  There is a statue of the god of Xiphia in the very front of the church.  You feel very safe in this very holy place.\n");
     set_exits(([ "out" : "/u/h/hamnsnock/pata/indoe2.c"
              ]));
}
