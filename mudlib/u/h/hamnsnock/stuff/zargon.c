#include <mudlib.h>
#include <weapon_types.h>

inherit WEAPON;

void create()
{
  ::create();
  set_weapon_info(({
  20,    //damage bonus
  20,    //damage range
  90,   //to_hit bonus (20 = 2%)
  90,   //Parry modifier
  ({"slice", "chop", "hack"}),     //hit_verbs
  ({"slices", "chops", "hacks"}),  //hit_verbs2
  ({"slice above", "chop beside", "narrowly miss"}),      //miss_verbs
  ({"slices above", "chops beside", "narrowly misses"}),  //miss_verbs_2
  ({"magical sword","deadly blade","evil weapon"}),        //weapon_names
  WEAPON_LONGSWORD,  //weapon_type
  this_object(),     //this object.  =>
  0,                 //hands used (not set here)
  WEAP_SKILL_LONGSWORD, //Skill weapon uses.
  }));
  set_hands(1);
  set_short("magical sword of zargon");
  set_long(
"This is a very deadly weapon.  It was created by the blackest of magic\n"
"by the evil magician Zargon.  This weapon is pure deadly evil and makes\n"
"anyone shake to even look at it\n");
  set_ids(({"sword","magical sword","zargon","weapon"}));
set_value(({ 100 , 4000 }));
}
