/*  -*- LPC -*-  */
// combat.c:  combat funcs inherited by user.c
//
// This file contains all functions pretaining to combat and 
// related realms.
// Orig by Casper 9/95
//
// 01.29.96  Tim  - Put prototypes in combat.h & included race.h protos
//                - Cleaned up a bit, took out this_object()'s since race.h
//                  now defines those functions
//                - Took out some unnecessary includes
//                - Changed create() to init_combat() so we don't need to
//                - play all these funky games with functions that aren't
//                  defined yet.  Just call this in the create func. of the
//                  child object.

#include <daemons.h> 
#include <weapon_types.h>
#include <race.h>
#include "combat.h"
 
private static int *ac,armours,free_hands, combating;
private static mixed *weapon_info; 
 
void init_combat()
{
  int i = query_hands()/2;
 
  set_ac(query_base_ac());
  set_armour(0);

  weapon_info = ({});
  while(i--)
    weapon_info += ({ query_unarmed(i) });

  combating = 0;
}

int query_num_attacks()
{
//This just patches combat so we CAN fight.  Fix over later.  Casper.
  return 1;
}
 
void set_combating(int foo)
{
  combating = foo;
}
 
int query_combating()
{
  return combating;
}
 
void set_ac(int *new_ac)
{
  if(ac)
  {
    ac[0] = (new_ac[0]==-1 ? ac[0] : new_ac[0]);
    ac[1] = (new_ac[1]==-1 ? ac[1] : new_ac[1]);
  }
  else
    ac = new_ac;
  if(combating)
    COMBAT_D->update_ac(this_object(),ac);
}

int *query_ac()
{
  return ac;
}
 
void set_armour(int new_armours)
{
  armours = new_armours;
}
 
int query_armour()
{
  return armours;
}
 
int query_free_hands()
{
  return free_hands;
}
 
void set_free_hands(int new_hands)
{
  free_hands = new_hands;
}
 
mixed *query_weapon_info()
{
  return weapon_info;
}
 
void set_weapon_info(mixed *new_weapon_info)
{
  weapon_info = new_weapon_info;
  if(combating)
    COMBAT_D->update_wc(this_object(), weapon_info);
}
 
//These next 4 functions handle the wearing & removing of 
//weapons and armour.  These do little or no checking of 
//arguments, so MAKE SURE they are right.  Doing this wrong 
//WILL (not just can) screw up the combat system.
//Orig by Casper
 
varargs int do_unwield(object weapon, int make_noise)
{
  int i, j, hands_info;
  mixed *weapon_info, *this_weapon_info;
 
  hands_info = query_free_hands();
  this_weapon_info = weapon->query_weapon_info();
  weapon_info = query_weapon_info();
  weapon_info -= ({ this_weapon_info });
  i = this_weapon_info[11]*2;
  if(weapon->query_hands()==2)
  {
    hands_info -= pow(2,i) + pow(2,i+1);
  }
  else
  {
    if(hands_info & (j = pow(2,i+1)))
      hands_info -= j;
    else
      hands_info -= j/2;
  }
  if(!hands_info || !weapon_info)  //We test both in case of a 
                                   //'bookkeeping' error.  Casper
  {
    weapon_info = ({});  //Redundant, but provides a possibly 
                         //necessary 'clean up point'  Casper
    for(i = 0,j = query_hands()/2;i < j;i++)
      weapon_info += ({ query_unarmed(i) });
  }
  set_free_hands(hands_info);
  set_weapon_info(weapon_info);
  weapon->set_wielded(0);

  tell_object(this_object(),
	      sprintf("You unwield %s.\n", weapon->short()));
  say(sprintf("%s unwields %s.\n",
	      this_object()->query_cap_name(),
	      weapon->short()));

  return 1;
}
 
varargs int do_wield(object weapon, int hands_used, int make_noise)
{
  mixed *weapon_info, *this_weapon_info;
  int max_hands = query_hands();
  int free_hands = query_free_hands();
  int i,j;
  int* foo;

// Bit sloppy here, but passing these args would be ugly, so
// I'll just derive them here.  Capser 10/9/95
  for(i = 0, j = 1; i < max_hands; j *= 2, i++)
  {
    if(hands_used & j)
    {
      j = i / 2;
      i = max_hands;
    }
  }
  weapon_info = weapon->query_weapon_info();
  this_weapon_info = query_unarmed(j,WEAPON_TYPE[weapon_info[12]]);
  this_weapon_info[0] += weapon_info[0];
  this_weapon_info[1] = weapon_info[1];
  this_weapon_info[2] += weapon_info[2];
  this_weapon_info[3] = weapon_info[3];
  this_weapon_info[4] = weapon_info[4];
  this_weapon_info[5] = weapon_info[5];
  this_weapon_info[6] = weapon_info[6];
  this_weapon_info[7] = weapon_info[7];
  this_weapon_info[8] = weapon_info[8];
  this_weapon_info[9] = weapon_info[9];
  this_weapon_info[10] = weapon;
  this_weapon_info[11] = j; //hand pair used.
 
  set_weapon_info((free_hands ? query_weapon_info() : ({}))
                  + ({ this_weapon_info }));
  set_free_hands(free_hands | hands_used);
 
  //Parry modifier.
  set_ac( ({ (foo = query_ac())[0] + this_weapon_info[3], foo[1] }) );
 
  if(make_noise)
  {
    tell_object(this_object(),
		sprintf("You wield %s.\n", weapon->short()));
    say(sprintf("%s wields %s.\n",
		this_object()->query_cap_name(),
		weapon->short()));
  }
  weapon->set_wielded(1);
  weapon->set_wield_info(this_weapon_info);

  return 1;
}
 
varargs int do_wear(object armour, int make_noise)
{
  string *armour_types,*armour_locations,armour_type;
  int i, j, armour_worn, *k,*m;
 
  i = member_array((armour_type=armour->query_type()), 
                   (armour_types = query_armour_types()));
  j = sizeof(armour_types);
  armour_worn = query_armour();

  while (i < j && (to_int(pow(2,i)) & armour_worn)
          && armour_types[i]==armour_type)
    i++;

  if((i == j) ? 1 : (armour_types[i]!=armour_type))
  {
    notify_fail("You have no place to wear "+armour->short()+".\n");
    return 0;
  }

  k = armour->query_ac();
  m = query_ac();
  m[0] += k[0];
  m[1] += k[1];
  set_ac(m);
  j = pow(2,i);
  set_armour(armour_worn + j);
  armour->set_worn(j, query_armour_locations()[i]);
  if(make_noise)
  {
    tell_object(this_object(),
		sprintf("You wear %s.\n", armour->short(1)));
    say(sprintf("%s wears %s.\n",
		this_object()->query_cap_name(),
		armour->short(1)));
  }

  return 1;
}
 
void do_remove(object armour, int make_noise)
{
  int i,*j,*k,m;
 
  i = armour->query_where_worn();
  j = query_ac();
  k = armour->query_ac();
  j[0] -= k[0];
  j[1] -= k[1];
  set_ac(j);
  set_armour(query_armour() - armour->query_where_worn());
  armour->set_worn(0,"");
  if(make_noise)
  {
    tell_object(this_object(),
		sprintf("You remove %s.\n", armour->short()));
    say(sprintf("%s removes %s.\n",
		this_object()->query_cap_name(),
		armour->short()));
  }
}
