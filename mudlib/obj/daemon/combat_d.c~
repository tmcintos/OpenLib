/*  -*- LPC -*-  */
// combatd.c
//
//This is the Combat Daemon.  It handles everything related to 
//combat that doesn't need to be in the player's body.
//
//Orig Daemon by Casper.  Mail at epeters@netcom.com for 
//questions concerning this daemon and any functions written by 
//me.  10/13/95 
//
// 10.23.95  Tim:  - fixed so combat is visible to bystanders (2 lines)
//                   amounted to putting in some tell_room()'s
//                 - removed possessive() cause it's an efun now;
 
#include <mudlib.h>
#include <object_types.h>
#include <weapon_types.h>
 
//Vars:  (save a lot of ugly code by using mappings)
 
private mapping combatants,weapon_info,ac,num_attacks;
//keys are their oids'
//combatants is an array, whos first element is the attacker, 
//and the remainder a list of who he/she is fighting.  
//Contains objs, not names.
 
//Functions:
//nessisary prototypes here.
varargs void execute_attack(object victim, object attacker, 
                       mixed *weapon_info, int hit_mod, int dmg_mod);
void execute_bow_attack(object victim, object attacker, mixed *weapon_info);
 
void create()
{
  combatants = ([]);
  weapon_info = ([]);
  ac = ([]);
  num_attacks = ([]);
}
 
//Stops ALL combat in progress, and reinitalizes all global vars.
void clear_all_combat_info()
{
  string *keylist = keys(combatants);
  int i = sizeof(keylist);
 
  //set everybody to non-combat status.
  while (i--)
    combatants[keylist[i]][0]->set_combating(0);
 
  combatants = ([]);
  weapon_info = ([]);
  ac = ([]);
  num_attacks = ([]);
}
 
//Return damage string, 'Stolen' from Rift (mod. TMI-2 lib) by 
//Casper  9/95
string *query_dmg_message(int damage)
{
  switch(damage) 
  {
    case 0:
      return ({"doing no damage.", "doing no damage."});
    case 1..10:
      return ({"leaving a light scratch.", 
         "leaving a light scratch."});
    case 11..20:
      return ({"doing light damage.", "doing light damage."});
    case 21..25:
      return ({"delivering a soft blow.", "delivering a soft blow."});
    case 26..35:
      return ({"delivering a solid blow.", "delivering a solid blow."});
    case 36..45:
      return ({"hitting hard!", "hitting hard!"});
    case 46..50:
      return ({"hitting VERY hard!", "hitting VERY hard!"});
    case 51..60:
       return({"smashing with a bone crunching sound!",
          "smashing with a bone crunching sound!"});
    case 61..70:
      return ({"inflicting massive damage!", "inflicting massive damage!"});
    case 71..80:
      return({"massacring to little fragments!",
         "massacring to little fragments!"});
    case 81..110:
      return ({"hitting with unbelievable force!",
         "hitting with unbelievable force!"});
    case 111..130: //sp on splatterring?
      return({"splatterring yourself with your opponent's blood!",
         "splatterring everything with blood!"});
    case 131..150:
      return({"spraying a thin red paste everywhere!",
         "spraying a thin red paste everywhere!"});
    default:
      return({"DESTROYING with your wicked blow!",
         "DESTROYING with the wicked blow!"});
  }
}
 
//Remove people from all attack lists that are not present.  
//Orig by Casper
int clean_up_all_attackers()
{
  string *keylist = keys(combatants);
  object *attackerlist;
  int i = sizeof(keylist),j;
 
  while(i--)
  {
    j = sizeof(combatants[keylist[i]]);
    attackerlist = ({combatants[keylist[i]][0]});
    if(attackerlist[0])
      while (--j)
      {
        if((combatants[keylist[i]][j] ? 
            (environment(combatants[keylist[i]][j]) == 
             environment(attackerlist[0])) : 0))
          attackerlist = attackerlist + ({combatants[keylist[i]][j]});
      }
    if(sizeof(attackerlist)==1)
    {
// this isn't working after 1 kill --Tim
//      {
//        tell_object(combatants[keylist[i]][0],"The combat ends.\n");
//      }
      if(combatants[keylist[i]][0])
        combatants[keylist[i]][0]->set_combating(0);
      map_delete(combatants,keylist[i]);
      map_delete(weapon_info,keylist[i]);
      map_delete(ac,keylist[i]);
      map_delete(num_attacks,keylist[i]);
    }     
    else combatants[keylist[i]] = attackerlist;
  }
  return sizeof(combatants);
}
 
//Try as heart_beat and call_out, checking cpu for each.
//This auctualy implements ALL the attacks.  Orig by Casper
void continue_all_attacks()
{
  string *keylist = keys(combatants);
  int i = sizeof(keylist), j,k,m;
 
  if (!clean_up_all_attackers())
  //No combat occuring?  aw, lets close down then.
    return;
  call_out("combat_crash",1); //time of 1 less than time 
                              //between attacks.
  while(i--)
  {
    k = num_attacks[keylist[i]];
    while (k--)
    {
      m = sizeof(weapon_info[keylist[i]]);
      while(m--)
      {
        if(weapon_info[keylist[i]][m][9] & WEAPON_BOW)
        {
          execute_bow_attack(combatants[keylist[i]][1],
            combatants[keylist[i]][0],
            weapon_info[keylist[i]][m]);
        }
        else
        {
          execute_attack(combatants[keylist[i]][1], 
            combatants[keylist[i]][0], 
            weapon_info[keylist[i]][m]);
        }
      }
    }
  }
  //Place this here so if we 'bomb' somewhere above, we don't 
  //repetitivly 'bomb'.  Casper
  call_out("continue_all_attacks",2);
  //For announcing the death of the daemon
  remove_call_out("combat_crash");
}
 
void combat_crash()
{
  shout("Combat Daemon shouts: Damn!\n");
  shout("Combat Daemon tells you: I've bombed.\n");
  log_file("combatd", "Combat daemon crashed on: " + ctime(time()) +
    ", by not completeing 'execute_all_attacks()'\n");
  destruct(this_object());
}
 
 
 
//Execute an attack.  Orig by Casper
varargs void execute_attack(object victim, object attacker, 
                    mixed *weapon_info, int hit_mod, int dmg_mod)
{
  int damage, to_hit, dmg_rnd, i, drunk_mod = victim->query_drunk() - attacker->query_drunk();
  string *message, pos = pronoun(attacker, 1), 
    vid = sprintf("%i",getoid(victim));
  string aname = attacker->query_cap_name(), vname = 
    victim->query_cap_name();
 
  //weapon_info is divided as follows:
  //0:  int damage bonus
  //1:  int damage range(0 to ??-1)
  //2:  int to_hit bonus
  //3:  int parry modifier
  //4:  string *hit_verbs (eg, slice, chop)
  //5:  string *hit_verbs2 (eg, slices, chops)
  //6:  string *miss_verbs (eg, slice above, swing wildly at, whatever)
  //7:  string *miss_verbs2 (eg, slices above, swings wildly at, whatever)
  //8:  string *weapon_names (eg, longsword, spear, pike, etc...)
  //9:  int weapon_type (eg, bow, short sword, long sword, etc)
  //10: object weapon_object
  //11: int hand_pair_used
 
  damage = dmg_mod + weapon_info[0] - ac[vid][1] 
  + drunk_mod; //(think unused args in a varargs come out as 0)
  to_hit = hit_mod + weapon_info[2] - ac[vid][0]
  + drunk_mod;
  i = random(sizeof(weapon_info[4]));
 
  if(to_hit < 20)
    to_hit = 20;
  if(to_hit > 980)
    to_hit =980;
  if(random(1000) < to_hit)
  { //We hit code in here
    if(damage > 0)
      damage = victim->recieve_damage(damage + random(weapon_info[1]));
    else
      damage = 0;
    message = query_dmg_message(damage);
    tell_object(attacker, sprintf("You %s %s with your %s, %s\n",
      weapon_info[4][i], vname, 
      weapon_info[8][i], message[0]));
    tell_object(victim, sprintf("%s %s you with %s %s, %s\n", 
      aname, weapon_info[5][i], pos, weapon_info[8][i], 
      message[1]));
    //Hmm, with a daemon, no wait, ahh... =>  Cheating here, 
    //better way to do this one?  Casper
    tell_room(environment(attacker), sprintf("%s %s %s with %s %s, %s\n",
      aname, weapon_info[5][i], vname, pos, weapon_info[8][i], 
      message[1]), ({ attacker, victim }));
    return;
  }
  //We missed code in here
  tell_object(attacker, sprintf("You %s %s with your %s.\n", 
    weapon_info[6][i], vname, weapon_info[8][i]));
  tell_object(victim, sprintf("%s %s you with %s %s.\n", aname, 
    weapon_info[7][i], pos, weapon_info[8][i]));
  tell_room(environment(attacker), sprintf("%s %s %s with %s %s.\n", aname, 
    weapon_info[7][i], vname, pos, weapon_info[8][i]), ({ attacker, victim}));
  return;
}
 
//Execute an attack with a bow.  Orig by Casper
void execute_bow_attack(object victim, object attacker, mixed *weapon_info)
{
  mixed *new_weapon_info;
  int j;
  object arrow;
 
  if(!(j = weapon_info[10]->do_arm_bow()))
  {
    execute_attack(victim,attacker,attacker->query_unarmed(weapon_info[11]),
     -100, -10);
    return;
  }
  if(j != -1) //-1 = ready to fire
    return;
  arrow = weapon_info[10]->query_arrow();
  weapon_info[10]->bow_fired();//Reset all info, dest the arrow, etc.
  new_weapon_info = arrow->query_weapon_info();
  new_weapon_info[0] += weapon_info[0];
  new_weapon_info[1] += weapon_info[1];
  new_weapon_info[2] += weapon_info[2];
  new_weapon_info[8] = weapon_info[8];
  execute_attack(victim,attacker,new_weapon_info);
}
 
//Start combat with person A against person B.  DO NOT CALL 
//DIRECTLY, LET kill_ob() and ONLY kill_ob() do this.  No exceptions.  //Orig by Casper
//No SUPER need to be efficent here.
void init_combat(object a, object b)
{
  string *keylist = keys(combatants), name = sprintf("%i",getoid(a));
 
  a->set_combating(1);
 
  if(member_array(name, keylist)==-1)
  {
    combatants[name] = ({a,b});
    weapon_info[name] = a->query_weapon_info();
    ac[name] = a->query_ac();
    num_attacks[name] = a->query_num_attacks();
    return;
  }
  if(!member_array(b,combatants[name]))
  {
    combatants[name] -= ({a});
    combatants[name] = ({a,b}) + combatants[name];
  }
  else
  {
    combatants[name] -= ({a});//inefficent??
    combatants[name] -= ({b});
    combatants[name] = ({a,b}) + combatants[name];
  }
  return;
}
 
 
//Check for valid targets & init combat.  if allready 
//fighting, the two will start slugging each other out, as 
//oposed to any other oponents they may have.  May want to chage 
//it so that only the attacker starts slugging the other out.
//Orig by Casper
int kill_ob(object attacker, object victim)
{
  int foo = victim->query_object_class();
 
  //Returns:
  // 1 for non-living target
  // 2 for attacking self
  // 3 for unallowed combat
  // 4 for victim not present
  // 0 on success
 
  if(!(foo && living(victim)))
    return 1;
  if(victim == attacker)
    return 2;
  if((foo & OBJECT_NO_ATTACK) || environment(victim)->
      query_object_class() & OBJECT_NO_ATTACK)
    return 3;
  if (environment(victim) != environment(attacker))
    return 4;
  if((foo && userp(victim)) & (!interactive(victim)))
    return 3;
  init_combat(attacker,victim);
  init_combat(victim,attacker);
  if (0 >= member_array(this_object(),call_out_info()))
    continue_all_attacks();
  return 0;
}
 
//These allow wield, wear, & stuff to update this info.  
//Auctual updates are handled in the combat body.  Casper 10/14/95
//Use only if fighting (query_combating() !=0) because no checks 
//here are made.
 
void update_ac(object tp, int *new_ac)
{
  ac[sprintf("%i",getoid(tp))]=new_ac;
}
 
void update_wc(object tp, mixed *new_wc)
{
  weapon_info[sprintf("%i",getoid(tp))]=new_wc;
}
