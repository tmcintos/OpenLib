/*  -*- LPC -*-  */
#include <mudlib.h>
#include <weapon_types.h>

inherit WEAPON;

void create()
{
  ::create();
  set_weapon_info(({
  25,   //damroll 
  100,  // damage range
  60,   //to_hit bonus 
  30,   //Parry modifier
  ({"slice", "chop", "hack"}),     //hit_verbs
  ({"slices", "chops", "hacks"}),  //hit_verbs2
  ({"slice above", "chop beside", "narrowly miss"}),      //miss_verbs
  ({"slices above", "chops beside", "narrowly misses"}),  //miss_verbs_2
  ({"doombringer","doom"})  //weapon_names
  WEAPON_LONGSWORD
  this_object(),     //this object.  =>
  1,    //hands used
  WEAP_SKILL_LONGSWORD, //Skill weapon uses.
  }));
  set_hands(1);

  set_short("Doombringer");
  set_long(@ENDLONG
This is Comintern's weapon of doom.
ENDLONG);

  set_ids(({"sword","long sword","longsword","weapon"}));
  set_weight(8);                                 // 8 lbs.(?) --Tim
  set_value(({ 40, 0 }));                        // 40 silver, 0 gold --Tim
}
