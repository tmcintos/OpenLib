//object containing functions pretaining to the race 'human'.
//Also example for others
//Orig by casper 9/95
 
#include <weapon_types.h>
 
string *query_armour_types()  //The 'ID' of armours that can be worn.
{
  return ({"helm","ring","ring","bracers","body2","legs2","boots",
           "amulet","cloak"});
}
 
string *query_armour_locations()  //Where the armours are worn
                                  //Parralel to above array
{
  return ({"on the head","on the left hand","on the right hand",
          "on the arms","about the body","on the legs",
          "on the feet","around the neck","around the neck"});
}
 
int *query_base_ac()
{
  return({0,0}); //({chance to hit mod(10 = 1%),damage mod})
}
 
int query_hands()  //The number of hands and therefore(?)
{                  //total number of weapons the race can wield
  return 2;  
}
 
//First int is which 'hand pair' is being queried about, the 
//second is 0 if raw score is wanted and 1 if skills for unarmed 
//combat are to be added.  Modifiers are described within.
//Humans have only one pair of hands, so the first int is 
//ignored.
 
mixed *query_unarmed(int not_really_important,int fflag)
{
  return ({
    this_object()->query_stat("strength") + (fflag ? 
      this_object()->query_skill("unarmed")/3 : 0)
    + this_object()->query_stat("agility")/3,
    //Strength does a 1:1 bonus to damage.  agility does a 3:1 
    //bonus (ability to direct the weapon where it hurts)
    (fflag ? 5 + this_object()->query_skill("unarmed")/2 : 0),
    //damage range needed ONLY for unarmed combat
    500 + this_object()->query_stat("strength") * 3 + 
      this_object()->query_stat("agility") * 10
    + (fflag ? this_object()->query_skill("unarmed") * 25 : 0),
    //500 = 50% base chance to hit * .3% per point of str + 1% 
    //per point of strength, unarmed combat skill addes 2.5% 
    //per point.
    (fflag ? 0 : this_object()->query_stat("strength") * 5 + 
      this_object()->query_stat("agility") * 10),
    //parry modifier.  Nill for unarmed combat (how DO you
    //parry with hands?) otherwize 1% per point of agil & .5%
    //per point of str.
    ({"punch","sock","chop","jab"}),
    ({"punches","socks","chops","jabs"}),
    ({"miss","swing above", "jab beside", "swing short"}),
    ({"misses","swings above", "jabs beside", "swings short"}),
    ({"fists","balled hand", "hand", "fist"}),
    WEAPON_FISTS,
    this_object(),
    0});
}
