//  -*- LPC -*- 
// monster.c  Tim, 10/95:   A very generic monster
//

#include <mudlib.h>

inherit LIVING;

private boolean move_around = FALSE;     // do we move around or not?
private boolean talking_monster = FALSE; // do we go around talking??
private int chance_talk = 100;           // chance to talk out of 100 
private int chance_act = 100;            // chance to move out of 100
private string *vocabulary = ({});       // lines that we speak

// Function Prototypes
void heart_beat();
void set_move_around(int chance);
void set_talk(int chance, string *lines);

private void random_action();
private void do_talk();

// Called by the driver every 2 seconds or so...

void
heart_beat()
{
  if(move_around && !random(chance_act))
    random_action();
  if(talking_monster && !random(chance_talk))
    do_talk();
}

// Call this to make the monster randomly move around and do things

void
set_move_around(int chance)
{
  chance_act = chance;
  move_around = TRUE;
}

// Take a random action...pick from all available actions to make it
// look real :)

void
random_action()
{
  mixed *actions = commands();
  int action = random(sizeof(actions));

  command(actions[action][0]);
}

// Call this to make the monster talk

void
set_talk(int chance, string *lines)
{
  chance_talk = chance;
  vocabulary = lines;
  talking_monster = TRUE;
}

// speak and be heard!!

void
do_talk()
{
  int num = random(sizeof(vocabulary));

  say(query_cap_name() + " says: " + vocabulary[num] +"\n");
}
