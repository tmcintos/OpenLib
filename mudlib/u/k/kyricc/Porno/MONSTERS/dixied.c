// Created by Kyricc, the local idiot.
// For PornoLand
// 21 Feb 96

#include <mudlib.h>
#include <gender.h>

inherit MONSTER "/human";

void
create()
{

  ::create();

  set_name("dixie");
  set_short("Dixie Dynamite");
  set_long(@ENDLONG
This is her...in all of her hugeness.  The woman with tits as large as the
smaller moons of Juipter.  You begin to stare and wonder how the both of
you can occupy such a small space.
ENDLONG);
  set_gender(GENDER_FEMALE);
  set_talk(85, ({"Oooooh Baby!!!",
                "Come suck my mammoth nipples.",
                "More than a handfull is a waste???",
                "Tittie gonna knock you out.",
              }));
  }
