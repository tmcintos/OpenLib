// Origional Mob by Comintern.
// Hangs with his homeys at Comintern HQ.
// 21 Feb 96

#include <mudlib.h>
#include <gender.h>

inherit MONSTER "/human";

void
create()
{

  ::create();

  set_name("marx");
  set_short("Karl Marx, founder of modern communism.");
  set_long(@ENDLONG
Standing before you is a man with a huge grey beard and intense eyes.  Yes,
this is the founder of the communist movement.  
ENDLONG);
  set_gender(GENDER_MALE);
  set_talk(40, ({"You seen Engles around here?",
                "Religion is the opiate of the masses.",
                "The workers should control the means of production.",
               "You have nothing to lose except your chains!",
                "Want a copy of my Manifesto?",
              }));
  }
