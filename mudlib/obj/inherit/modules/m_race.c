/*  -*- LPC -*-  */
// m_race.c:  race specific code
//
// Functions pretaining to the race 'clay'.
// Malieable race, for all forms of animals and creatures
// Orig by casper 11/95

#include <weapon.h>
 
int query_stat(string);
int query_skill(string);

private static string race_name;
private static int base_weight;
private static int hands;
private static string *armour_types, *armour_locations;
private static int *base_ac;
private static function combat_info;
private static int base_str, base_agil;

int str, agil;       // stats so far
mapping skills;      // easy way out

void init_race(mixed* raceinfo)
{
  race_name = raceinfo[0];
  hands = raceinfo[1];
  base_weight = raceinfo[2];
  base_str = raceinfo[3];
  base_agil = raceinfo[4];
  armour_types = raceinfo[5];
  armour_locations = raceinfo[6];
  base_ac = raceinfo[7],
  combat_info = bind(raceinfo[8], this_object());
}

int
query_stat(string stat)
{
  switch( stat )
  {
  case "strength":
    return base_str + str;
  case "agility":
    return base_agil + agil;
  default:
    return 0;
  }
}

int
query_skill(string skill)
{
  // put guts in here too :) 5-18 for users 5-25 monsters
  return 11;
}

string query_race()
{
  return race_name;
}

int
query_base_weight()
{
  return base_weight;
}

string *query_armour_types()  //The 'ID' of armours that can be worn.
{
  return copy(armour_types);
}
 
string *query_armour_locations()  //Where the armours are worn
                                  //Parralel to above array
{
  return copy(armour_locations);
}
 
int *query_base_ac()
{
  return copy(base_ac);
}
 
int query_hands()  //The number of hands and therefore(?)
{                  //total number of weapons the race can wield
  return hands;
}
 
varargs mixed *query_unarmed(int hand_pair,string wep_skill)
{
  int str = query_stat("strength"),
      agl = query_stat("agility"),
      cbt = query_skill("combat"),
      unr = query_skill("unarmed"),
      ws = query_skill(wep_skill);
 
  return evaluate(combat_info, hand_pair, wep_skill, str, agl, cbt, unr, ws);
}

//
// override of long()
//
string
long()
{
  return sprintf("%s is a member of the %s race.\n",
		 capitalize(pronoun(this_object(), 0)), race_name);
}
