/*  -*- LPC -*-  */
/* -=Coded by the village idiot Kyricc=- */

#include <mudlib.h>
#include <weapon_types.h>

inherit WEAPON;

void create()
{
  ::create();
  set_weapon_info(({
  50,    //damage bonus
  60,    //damage range
  500,   //to_hit bonus (20 = 2%)
  400,   //Parry modifier
  ({"liquify", "disentegrate", "creamate"}),     //hit_verbs
  ({"liquifies", "disentegrates", "creamates"}),  //hit_verbs2
  ({"slice above", "chop beside", "narrowly miss"}),      //miss_verbs
  ({"slices above", "chops beside", "narrowly misses"}),  //miss_verbs_2
  ({"dick","schlong","man-meat"}),
  WEAPON_DAGGER,  //weapon_type
  this_object(),     //this object.  =>
  0,                 //hands used (not set here)
  WEAP_SKILL_DAGGER, //Skill weapon uses.
  }));
  set_hands(1);
  set_short("John Holms' 3 foot dick");
  set_long(@ENDLONG
This is one massive piece of meat.  I bet it does a lot of damage too!
ENDLONG);
  set_ids(({"dick", "jdick"}));
  set_value(({ 40, 0 }));                        // 40 silver, 0 gold --Tim
}

