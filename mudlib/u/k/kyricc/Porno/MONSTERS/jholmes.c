// Coded by the local idiot Kyricc
// Made for PornoLand
// 22 Feb 96

#include <mudlib.h>
#include <gender.h>

inherit MONSTER "/human";

void
create()
{

  object dick;

  ::create();

  set_name("john");
  set_short("John Holmes");
  set_long(@ENDLONG
This is a REAL man.  Thats is the man with the biggest dick.
ENDLONG);
  set_talk(25, ({"Suck it bitch!",
                 "Ohhh it's getting hard.",
                 "I see your schwartz is as big as mine!",
               }));

  dick = new("/u/k/kyricc/Porno/OBJ/jdick.c");
  dick->move(this_object());
  do_wield(dick, 1, 0);
}
