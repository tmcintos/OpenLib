//This file contains all functions pretaining to combat and 
//related realms.
//Orig by Casper 9/95
 
#include <mudlib.h>
#include <daemons.h> 
#include <weapon_types.h>
 
void set_ac(int *new_ac);
void set_armour(int new_armours);
 
private static int *ac,armours,free_hands, combating;
private static mixed *weapon_info; 
 
void create()
{
  int i = this_object()->query_hands()/2;
 
  set_ac(this_object()->query_base_ac());
  set_armour(0);
  weapon_info = ({});
  while(i--)
    weapon_info += ({this_object()->query_unarmed(i)});
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
  int i,hands_info,j;
  mixed *weapon_info, *this_weapon_info;
 
  hands_info = query_free_hands();
  this_weapon_info = weapon->query_weapon_info();
  weapon_info = query_weapon_info();
  weapon_info -= ({this_weapon_info});
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
                         //nessisary 'clean up point'  Casper
    for(i = 0,j = this_object()->query_hands()/2;i < j;i++)
      weapon_info += ({this_object()->query_unarmed(i)});
  }
  set_free_hands(hands_info);
  set_weapon_info(weapon_info);
  weapon->set_wielded(0);
    tell_object(this_object(), "You unwield "+weapon->short()+".\n");
    tell_room(environment(this_object()), this_object()->query_cap_name()+" unwields "+ weapon->short()+".\n", ({this_object()}));
  return 1;
}
 
varargs int do_wield(object weapon, int hands_used, int make_noise)
{
  mixed *weapon_info, *this_weapon_info;
  int i,j,max_hands = this_object()->query_hands(), free_hands = query_free_hands();
  int *foo;
//Bit sloppy here, but passing these args would be ugly, so
//I'll just derive them here.  Capser 10/9/95
  for(i = 0, j = 1; i < max_hands; j *= 2, i++)
  {
    if(hands_used & j)
    {
      j = i / 2;
      i = max_hands;
    }
  }
  weapon_info = weapon->query_weapon_info();
  this_weapon_info = this_object()->query_unarmed(j,WEAPON_TYPE[weapon_info[12]]);
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
 
  set_weapon_info((free_hands ? query_weapon_info() : 
                         ({})) + ({this_weapon_info}));
  set_free_hands(free_hands | hands_used);
 
  //Parry modifier.
  set_ac(({(foo = query_ac())[0]+this_weapon_info[3],foo[1]}));
 
  if(make_noise)
  {
    tell_object(this_object(), "You wield "+weapon->short()+".\n");
    tell_room(environment(this_object()), "You wield "+weapon->short()+".\n", ({this_object()}));
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
                   (armour_types = this_object()->query_armour_types()));
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
  armour->set_worn(j,this_object()->query_armour_locations()[i]);
  if(make_noise)
  {
    tell_object(this_object(), "You wear "+armour->short(1)+".\n");
    tell_room(environment(this_player()), this_object()->query_cap_name()+" wears "+armour->short(1)+".\n", ({this_object()}));
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
    tell_object(this_object(), "You remove "+ armour->short()+".\n");
    tell_room(environment(this_object()), this_object()->query_cap_name()+" removes "+armour->short()+".\n", ({this_object()}));
  }
}
