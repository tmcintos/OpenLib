#include "std.h"
#include <mudlib.h>
#define NAME "Damian"
object cape;
int door_is_open;

inherit ROOM;
void
create()
{

::create();
set_light(1);
set_short("The residence of Damian");
set_long("You have arrived at the residence of Damian.  A dense\n"+
         "fog darkens the outher perimeter of the room, while an\n"+
         "eerie glow emminates from a large fiery pit centered in\n"+
         "this chamber.  The room is scattered with skulls from various\n"+
         "type of creatures.  You feel a tremendous sense of evil lurking\n"+
         "about.  There appears to be no obvious exits, except for an\n"+
       "blood covered opening in the wall...\n");
  if (door_is_open)
  {
    write("The flames reveal a small passageway, leading to\n");
    write("to some sort of Dungeon.\n");
    return;
  }
}
reset(started) {
if (!started)
set_light (1);
  if (!cape) {
    cape = clone_object("/u/d/damian/obj/armors/cape");
    cape->set_name("cloak");
    cape->set_short("Cloak of the Shadow");
    cape->value(1);
    cape->weight(1);
    cape->set_ac(1000);
    cape->set_alias("cloak");
    cape->set_type("cloak");
    cape->set_cape_func(this_object());
  cape->move(this_object());
  }
door_is_open = 0;
}
init() {
  add_action("open","open");
  add_action("close","close");
  add_action("enter","enter");
}
open(str) {
 if(str !="flame")
  return 0;
  if (door_is_open)
  return 0;
  door_is_open = 1;
  write("The flame opens to reveal a hidden passageway.\n");
  say("The flame opens to reveal a hidden passageway.\n");
  return 1;
}
close (str) {
  if (str!= "flame")
return 0;
  if (!door_is_open)
return 0;
  door_is_open = 0;
  write("The passageway is engulfed by the flames.\n");
  say("The passageway is engulfed by the flames.\n");
  return 1;
}
enter (str) {
  if (str != "flame")
return 0;
if (!door_is_open)
return 0;
call_other(this_player(),"move_player","into the flame#/u/d/damian/rooms/flame.c");
return 1;
}
cape_func() {
  if (!this_player()->query_invis()) {
   say(capitalize(this_player()->query_name())+"'s cloak swirls around "+
   this_player()->query_objective() + " as " +
   this_player()->query_pronoun()+ " fades away.\n");
   write("You disappear in the folds of your cloak.\n");
   this_player()->invis();
   return 1;
}
this_player()->vis();
  say(capitalize(this_player()->query_name())+"'s cloak falls about "+
  this_player()->query_possessive() +" shoulders as " + this_player()
  ->query_pronoun() + " fades into existence.\n");
  write("You emerge from the folds of your cloak.\n");
  return 1;
}
