#include <mudlib.h>
 
inherit ARMOUR;
 
void create()
{
  ::create();
  set_short("a suit of magical chainmail");
  set_long(
"This chainmail looks wimpy indeed, but it was created by the black evil\n"
"magic of Zargon.\n");
  set_ac(600,15);   //makes 10% harder to hit, and reduces damage 
                   //delivered by 8
  set_type("body2");  //2 denotes for 2 armed creature.
  set_ids(({"chainmail", "chain", "mail", "suit of chainmail", 
            "suit", "armour", "armor"}));
}
