//Example armour.
//Orig by Casper 9/95
#include <mudlib.h>
 
inherit ARMOUR;
 
void create()
{
  ::create();
  set_short("a suit of chainmail");
  set_long(
"A suit of armour made from interlocking rings of metal,\n"
"heavy, but not a much so as plate, making it ideal for quick\n"
"incursions into battle.\n");
  set_ac(100,8);   //makes 10% harder to hit, and reduces damage 
                   //delivered by 8
  set_type("body2");  //2 denotes for 2 armed creature.
  add_ids(({"chainmail", "chain", "mail", "suit of chainmail", 
            "suit", "armour", "armor"}));
}
