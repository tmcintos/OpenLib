#include <mudlib.h>
#include <weapon_types.h>

inherit WEAPON;

void setup()
{
  add_ids( ({"whip","sexy whip"}) );
  set_short("a sexy whip");
  set_long(@ENDLONG
The whip is long and sexy looking.
ENDLONG);

  set_weight(8);                                 // 8 lbs.(?) --Tim
  set_bulk(3);                                   // who knows?
  set_value(({ 40, 0 }));                        // 40 silver, 0 gold --Tim

  set_weapon_info(({
  5,    //damage bonus
  6,    //damage range
  20,   //to_hit bonus (20 = 2%)
  30,   //Parry modifier
  ({"slice", "chop", "hack"}),     //hit_verbs
  ({"slices", "chops", "hacks"}),  //hit_verbs2
  ({"slice above", "chop beside", "narrowly miss"}),      //miss_verbs
  ({"slices above", "chops beside", "narrowly misses"}),  //miss_verbs_2
  ({"longsword","sharp blade","polished weapon"}),        //weapon_names
  WEAPON_LONGSWORD,  //weapon_type
  this_object(),     //this object.  =>
  0,                 //hands used (not set here)
  WEAP_SKILL_LONGSWORD, //Skill weapon uses.
  }));
  set_hands(1);
}
