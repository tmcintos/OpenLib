#include <mudlib.h>

inherit WEAPON;


object person;
object portal;
reset(arg){
   if(arg)
     return;
}

init(){
  add_action( "send_portal" , "sendp" );
  add_action( "smush_ring" , "smush" );
}


create(){
  seteuid(getuid());
}
long(){
   return "Your looking at a magical golden ring.";
}

short(){
   return "A magical golden ring";
}

id(str){
   return str == "ring";
}

smush_ring(string str){
if(!str){
   write("Smush what?\n");
   return 1;
}
if(str == "ring" || str == "golden ring"){
   say(this_player()->query_cap_name() + " drops a ring, and smushes it into the ground.\n");
   write("You drop the ring, and smush it into the ground.\n");
   destruct(this_object());
   return 1;
}
write("Smush only smushes magical golden rings.\n");
return 1;
}

send_portal(string arg){
if(!arg){
    write("Send the portal to who?\n");
    return 1;
}
person = find_player(arg);
if(!person){
   write("The ring does not know who that is.\n");
   return 1;
}
  portal = new("/u/h/hamnsnock/stuff/portal.c");
   move_object(portal, environment(person));
  tell_object(environment(person), "A golden portal forms in front of you.\n");
  tell_object(this_player(), "Your ring glows as a portal is sent off.\n");
  say(this_player()->query_cap_name() + "'s golden ring glows.\n");
}

