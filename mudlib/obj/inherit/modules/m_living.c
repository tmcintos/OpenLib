/*  -*- LPC -*-  */
// m_living:  Functions for living objects
// Tim 24 Apr 96

#include <dirs.h>
#include "m_living.h"

//
// Global Variables
//
private static string name;            // our name, don't save in users
private static int gender;             // 1=male -1=female 0=neutral (gender.h)
private int hit_points, spell_points;  // our HP/SP

void
low_setup()
{
  set_prevent_get(1);
  hit_points = 100;          // temporary! for testing death
  enable_commands();         // living object; Tim 9/14/95
  set_heart_beat(1);
}

void
remove()
{
  foreach(object ob in all_inventory(this_object())) {
    string sh = ob->short();

    if( ob->drop() && ob->move(environment()) ) {
      tell_object(this_object(), sprintf("You drop %s.\n", sh));
      tell_room(environment(),
		sprintf("%s drops %s.\n", query_cap_name(), sh),
		this_object());
    }
  }
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

nomask string
query_name()
{
  return name;
}

string
query_cap_name()
{
  return capitalize(name);
}

nomask void
set_gender(int s)
{
  gender = s;
}

nomask int
query_gender()
{
  return gender;
}

int
query_stat(string stat)
{
  // put guts in here :)
  return 5;
}

int
query_skill(string skill)
{
  // put guts in here too :)
  return 5;
}

nomask int
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
id(string arg)
{
  return arg == name;
}

int
receive_object(object ob)
{
  // test for strength and room to carry object
  return 1;
}

// Death :)
// Need to overload this for players

void
die()
{
  object corpse;

  tell_object(this_object(), "You die.\n"
	  "You see your own dead body from above.\n");

  tell_room(environment(), sprintf("%s hits the ground with a thud.\n"
	      "Some mist rises from the body.\n", query_cap_name()));

  corpse = new(CLONE_DIR "/corpse");
  corpse->set_short(short());

  foreach(object ob in all_inventory(this_object())) {
    if(ob->drop())
      ob->move(corpse);
  }

  corpse->move(environment(this_object()));

  destruct();
}
