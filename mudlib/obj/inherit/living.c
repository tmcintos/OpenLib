/*  -*- LPC -*-  */
// living.c:  Tim:  10/03/95
//   Originally by Tim with a couple lines from MudOS/lil's user.c
//
// 10/11/95     Tim:  Changed to inherit OBJECT, removed and changed some funcs
//                    as a result of this change.
// 10/14/95     Casper:  Including a generic race body for use with monsters.
// 10/14/95     Tim:  added weight and money funcs.
// 10/16/95     Tim:  started adding support for DEATH >evil grin<
// 04/20/96     Tim:  changed to inherit CONTAINER, took out add_weight()
//                    changed long() to return formatted string only.
//                    Updated w.r.t. object.c

#pragma save_binary

#include <mudlib.h>
#include <object_types.h>
#include <money.h>

inherit INHERIT_DIR "/combat";
inherit CONTAINER;

#include <race.h>  // race prototypes

// Global Variables
private static string name;            // our name, don't save in users
private static int gender;             // 1=male -1=female 0=neutral (gender.h)

private int *coins;                    // our money
private int hit_points, spell_points;  // our HP/SP

/*
 * Function Prototypes
 */
void create();
int set_name(string name);
int id(string id);
nomask int add_money(int* money);
nomask int remove_money(int* money);
nomask int get();
string long();
void die();

/*
 * Query functions
 */
string query_name() { return name; }
string query_cap_name() { return capitalize(name); }
int* query_money() { return copy(coins); }
int query_gender() { return gender; }

int
query_stat(string stat)
{
  // put guts in here :)
  return 5;
}

int
recieve_damage(int damage)
{
  write(sprintf("You took %i damage.\n",damage));

  if(!hit_points)
    return 0;

  hit_points -= damage;

  if(hit_points < 0)
    hit_points = 0;

// don't die if we're a player right now -- Tim
  if( !userp(this_player()) )
    if(!hit_points)
      call_out((: die :), 2);

  return damage;
}

int
query_skill(string skill)
{
  // put guts in here too :)
  return 5;
}

/*
 * Rest of the Implementation
 */
void
create()
{
  ::create();

  if( clonep() ) {
    coins = allocate(COIN_TYPES);
    for(int i = 0; i < COIN_TYPES; i++)
      coins[i] = 0;

    enable_commands();         // living object; Tim 9/14/95
    set_heart_beat(1);

    init_combat();

    hit_points = 100;       // temporary! for testing death
  }
}

int
clean_up(int inh)
{
  if( !inh && !environment() )
    destruct(this_object());
  else
    return 1;
}

/*
 * set_name:  Set the name of this living object
 */
int
set_name(string namestr)
{
  name = lower_case(namestr);
  set_living_name(name);          // for find_living()
  return 1;
}

nomask void
set_gender(int s)
{
  gender = s;
}

/*
 * add money to living -- returns 1 if the money was received successfully
 */
nomask int
add_money(int *money)
{
  int weight;

  if( sizeof(money) < COIN_TYPES )
    return 0;

  weight += money[SILVER] / 18;
  weight += money[GOLD] / 6;

  if( weight + query_weight_contained() > query_weight_capacity() )
    return 0;

  for(int i = 0; i < COIN_TYPES; i++)
    coins[i] += money[i];

  return 1;
}

/*
 * remove money from living--makes change
 */
nomask int
remove_money(int *money)
{
  int remove_silver, remove_gold;

  if(sizeof(money) < COIN_TYPES)
    return 0;

  if( coins[SILVER] >= money[SILVER] ) {
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

  if( coins[GOLD] >= money[GOLD] ) {
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

/*
 * called by the present() efun (and some others) to determine whether
 * an object is referred as an 'arg'.
 */
int
id(string arg)
{
  return ( arg == query_name() || ::id(arg) );
}

/*
 * Overload of get()...You can't get a living object
 * no need to overload drop() since you can't get it :)
 */
nomask int
get()
{
  return 0;
}

/*
 * Long description
 */
string
long()
{
  object* obs = all_inventory(this_object());
  string prn = capitalize(pronoun(this_object(), 0));
  string ret = short() + ".\n" + (long_desc ? long_desc : "");
  string tmp;

  ret += sprintf("  %s is a %s.\n", prn, query_race());

  /*
   * Inventory
   */
  tmp = sprintf("  %s is carrying ", prn);

  if( sizeof(obs) )
    tmp += list_obs(obs) + ".\n";
  else
    tmp += "nothing.\n";

  ret += break_string(tmp, 70);
  return ret;
}

int
receive_object(object ob)
{
  // test for strength and room to carry object
  return 1;
}

/*
 * overload of container::query_weight_contained()
 */
int
query_weight_contained()
{
  int weight = ::query_weight_contained();     // body wt. + inventory wt.

  // weight of coins
  weight += coins[SILVER] / 18;
  weight += coins[GOLD] / 6;

  return weight;
}
  
// Death :)
// Need to override this for players

void
die()
{
  object corpse, ob;

  tell_object(this_object(), "You die.\n"
	  "You see your own dead body from above.\n");

  tell_room(environment(), sprintf("%s hits the ground with a thud.\n"
	      "Some mist rises from the body.\n", query_cap_name()));

  corpse = new(CLONE_DIR "/corpse");
  corpse->set_short(short());

  foreach(ob in all_inventory(this_object())) {
    if(ob->drop())
      ob->move(corpse);
  }

  corpse->move(environment(this_object()));

  destruct(this_object());
}
