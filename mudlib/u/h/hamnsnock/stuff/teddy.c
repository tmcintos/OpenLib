#include <mudlib.h>

inherit ARMOUR;

void setup()
{
  add_ids( ({"teddy" , "slinky" , "tedd"}) );
  set_short("a red slinky teddy");
  set_long("A red slinky teddy.\n");

  set_weight(20);  // 20 lbs.
  set_bulk(4);  // who knows what these units are?
  set_value(({ 20, 1 }));   // worth 70 silver (?) -- Tim

  set_ac(100,8);  // makes 10% harder to hit, and reduces damage delivered by 8
  set_type("body2");  // 2 denotes for 2 armed creature.
}
