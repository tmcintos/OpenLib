#include <mudlib.h>

inherit MONSTER "/human";

void create()
{
    object weap;
    ::create();
    set_name("Guard");
    set_short("Village Guard");
    set_long("A tall guard that greets the people leaving and entering the village.\n");
set_talk(5, ({"Welcome to Patagonia!",
             "Welcome, enjoy your visit!",
           }));
    weap = new("/obj/examples/weapon.c");
    weap->move(this_object());
    do_wield(weap, 1, 1);
}
