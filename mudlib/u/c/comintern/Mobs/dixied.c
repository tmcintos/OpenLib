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

  set_name("stalin");
  set_short("Joseph Stalin, leader of the Soviet Union."
  set_long(@ENDLONG
You don't want to mess with this bad motherfucker.  Just on look in 
his eyes is enough to make you shudder with fear.  
ENDLONG);
  set_gender(GENDER_MALE);
  set_talk(50, ({"Off to the Gulag, you traitor!",
                "You German bastards, I'll kick your asses back to Berlin!",
                "It's about time for another 5 year plan.",
                "Webbed feet?  Who has webbed feet?",
                "Trotsky can be a real asshole sometimes.",
                "Hell, it's only a couple million people.  They'll grow back.",
                "I AM the Soviet Union.",
              }));
  }
