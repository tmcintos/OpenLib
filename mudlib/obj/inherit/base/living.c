/*  -*- LPC -*-  */
// living.c:  Tim:  10/03/95
//   Originally by Tim with a couple lines from MudOS/lil's user.c
//
// 10/11/95     Tim:  Changed to inherit OBJECT, removed and changed some funcs
//                    as a result of this change.
// 10/14/95     Casper:  Including a generic race body for use with monsters.
// 10/14/95     Tim: added weight and money funcs.
//

#include <mudlib.h>
#include <object_types.h>
#include <money.h>
#include <races/human.h>

inherit INHERIT_DIR "/combat";
inherit OBJECT;

// Global Variables
private string name = "";              // our name
private int *coins;                    // our money

// Function Prototypes
void create();
int set_name(string name);
int id(string id);
int get();
varargs mixed long(int flag);
void fix_weight();                     // re-init the weight of this obj
int add_weight(int weight);            // add weight to living (if we can)

// Query functions
string query_name() { return name; }
string query_cap_name() { return capitalize(name); }
int *  query_money() { return coins; }

int
query_stat(string stat)
{
  // put guts in here :)
  return 5;
}

int recieve_damage(int damage)
{
  tell_object(this_object(),sprintf("You took %i damage.\n",damage));
  return damage;
}

int
query_skill(string skill)
{
  // put guts in here too :)
  return 5;
}

// Rest of the Implementation

void
create()
{
  object::create();
  object::set_object_class(OBJECT_LIVING | OBJECT_CONTAINER);
  coins = allocate(COIN_TYPES);
  for(int i = 0; i < COIN_TYPES; i++)
    coins[i] = 0;

  enable_commands();         // living object; Tim 9/14/95
  set_heart_beat(1);         // enable heart beat

  combat::create();
}

// set_name:  Set the name of this living object

int
set_name(string namestr)
{
  name = lower_case(namestr);
  set_living_name(namestr);          // for find_living()
  return 1;
}

// add money to living -- returns 1 if the money was received successfully

int
add_money(int *money)
{
  int i, weight = 0;

  if(sizeof(money) < COIN_TYPES)
    return 0;

  weight += money[SILVER] / 18;
  weight += money[GOLD] / 6;

  if(!add_weight(weight))
    return 0;

  for(i = 0; i < COIN_TYPES; i++) {
    coins[i] += money[i];
  }

  fix_weight();
  return 1;
}

//remove money from living
int
remove_money(int *money)
{
  int remove_silver, remove_gold;

  if(sizeof(money) < COIN_TYPES)
    return 0;

  if(coins[SILVER] >= money[SILVER]) {
    remove_silver = money[SILVER];
  } else {
    int gold_required, left_over;

    money[SILVER] -= coins[SILVER];

    gold_required = 1 + (money[SILVER] - 1) / ONE_GOLD_IN_SILVER;
    left_over = money[SILVER] % ONE_GOLD_IN_SILVER;

    if(coins[GOLD] >= gold_required + money[GOLD]) {
      coins[GOLD] -= gold_required + money[GOLD];
      coins[SILVER] = ONE_GOLD_IN_SILVER - left_over;
      return 1;
    } else {
      return 0;
    }
  }

  if(coins[GOLD] >= money[GOLD]) {
    remove_gold = money[GOLD];
  } else {
    int silver_required;

    money[GOLD] -= coins[GOLD];

    silver_required = money[GOLD] * ONE_GOLD_IN_SILVER;

    if(coins[SILVER] >= silver_required) {
      coins[SILVER] -= silver_required + money[SILVER];
      coins[GOLD] = 0;
      return 1;
    } else {
      return 0;
    }
  }

  coins[GOLD] -= remove_gold;
  coins[SILVER] -= remove_silver;

  return 1;
}

// called by the present() efun (and some others) to determine whether
// an object is referred as an 'arg'.

int
id(string arg)
{
  if(!query_name()) 
    return arg == "living";
  return arg == query_name();
}

// Overload of get()...You can't get a living object
//   no need to overload drop() since you can't get it :)

int
get()
{
  return 0;
}

// Long description
// with flag set = 1, this returns a string
// otherwise writes it out

varargs
mixed
long(int flag)
{
  object ob;

  if(!flag) {
    write(short() +"\n" +
	  (long_desc ? long_desc : "") +
	  "He/She/It is carrying:\n");
    
    foreach(ob in all_inventory(this_object())) {
      string tmp;

      if(tmp = (string)ob->short())
	write("  "+ capitalize(tmp) +"\n");
    }

    return 1;
  } else {
    string ret = "";

    ret += short() + "\n" +
           (long_desc ? long_desc : "") +
           "He/She is carrying:\n";

    foreach(ob in all_inventory(this_object())) {
      string tmp;

      if(tmp = (string)ob->short())
	ret += "  " + tmp + "\n";
    }

    return ret;
  }
}

// re-init the player's weight

void
fix_weight()
{
  int my_weight;
  object ob;

// My base body weight
  my_weight = this_object()->query_base_weight();

// weight added by each object I'm carrying
  foreach(ob in all_inventory(this_object())) {
    my_weight += ob->query_weight();
  }

// weight from coins
  my_weight += coins[SILVER] / 18;
  my_weight += coins[GOLD] / 6;

  set_weight(my_weight);
}

boolean
add_weight(int pounds)
{
  return TRUE;           // remove later -- Tim

  if((query_weight() + pounds) > this_object()->query_max_weight())
    return FALSE;

  return TRUE;
}
