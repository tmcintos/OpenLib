/* -*- LPC -*- */
/* -=Coded by the local idiot Kyricc=- */

#include <mudlib.h>
 
inherit ARMOUR;
 
void create()
{
  ::create();
  set_short("Kyricc's cloak of Darkness");
  set_long(
"This is Kyricc's cloak of Darkness.  It is made of the finest\n"
"silk, and is so black it seems to absorb light.  It has mystic\n"
"runes stitched in gold thread around the hood.  This garment\n"
"gives you a chill from just being in the same room as it.\n");
  set_ac(800,10);   //makes 10% harder to hit, and reduces damage 
                   //delivered by 8
  set_type("body2");  //2 denotes for 2 armed creature.
  add_ids(({"kcod", "k cloak", "darkman"}));
  set_value(({0,2000}));
}
