#include <mudlib.h>

inherit OBJECT;


object victum;
string arrive_msg = this_player()->query_cap_name() + " appears in a puff of smoke.";
string depart_msg = this_player()->query_cap_name() + " disappears in a puff of smoke.";
reset(arg){
  if(arg)
    return;
}


string old_long;
int in_hand = 0;
long(){
   write("A magical wand that belongs to Hamnsnock.\n");
   return 1;
}

short(){
 if(in_hand){
   return "a magic wand (held in right hand)";
}
if(!in_hand){
   return "a magic wand";
}
}

id(str){
   return str == "wand";
}

init(){
    add_action( "do_hold" , "hold" );
    add_action( "do_snuff" , "snuff" );
   add_action( "do_join" , ".join" );
   add_action( "do_change_arrive" , ".setarrive" );
    add_action( "do_change_depart" , ".setdepart" );
   add_action( "do_go_home" , ".home" );
    add_action( "do_status" , ".status" );
    add_action( "do_goto" , ".goto" );
    add_action( "do_quit" , ".quit" );
}

      
do_hold(string str){
  if(!str){
     write("Hold What?\n");
     return 1;
}
if(environment(this_object()) == this_player()){
  if(str == "wand"){
   if(!in_hand){
   in_hand = 1;
 write("You hold the wand in your right hand.\n");
 say(this_player()->query_cap_name() + " holds a magic wand.\n");
   return 1;
}
   write("You are already holding the wand!\n");
   return 1;
}
   write("Heh, you still don't have that.\n");
   return 1;
} 
  write("You don't have that.\n");
  return 1;
}

do_snuff(string str){
if(!str){
   write("Snuff what?\n");
  return 1;
}
if(str == "wand"){
    say("A magic wand flys out of the room.\n");
  write("You snuff the magic wand, and it is gone.\n");
    destruct(this_object());
    return 1;
}
else {
    write("Only can snuff magic wands, sorry.\n");
    return 1;
}
}

do_join(string str){
if(!str){
   write("The wand glows and whispers, \"Join who?\"\n");
   return 1;
}
victum = find_player(str);
if(!victum){
   write("The wand can't find that person.\n");
   return 1;
}
  if(victum == this_player() || environment(victum) == environment(this_player())){
    write("The wand slaps you for trying to do something that stupid.\n");
    victum = 0;
     return 1;
}
  tell_object(environment(victum), arrive_msg + "\n");
 say(depart_msg + "\n");
this_player()->move(environment(victum));
tell_object(this_player(), "You wave your wand and you are transported to " + victum->query_cap_name() + ".\n");
  this_player()->force_me("look");
return 1;
}

do_change_arrive(string message){
if(!message){
     write("The wand says your arrive message is: " + arrive_msg + "\n");
     return 1;
}
arrive_msg = message;
write("The wand says your arrive message is now: " + arrive_msg + "\n");
write("   --Hope your included your name--\n");
     return 1;
}
do_change_depart(string depart){
if(!depart){
   write("The wand says your depart message is: " + depart_msg + "\n");
   return 1;
}
depart_msg = depart;
write("The wand says your depart message is now: " + depart_msg + "\n");
write("  --Hope your included your name--\n");
    return 1;
}

do_go_home(){
 object home;
 string workroom;

   workroom = RESOLVE_PATH("~/workroom");

   home = load_object(workroom);
if(!home){
   write("The wand can't find your home.\n");
   return 1;
}
if(environment(this_player()) == home){
    write("The wand think you are already home.\n");
    return 1;
}
   say(depart_msg + "\n");
   this_player()->move(workroom);
   say(arrive_msg + "\n");
   write("You wave the wand and return home.\n");
   this_player()->force_me("look");
return 1;
}

do_status(){
write("You rub the wand, and a little screen appears.....\n");
write(sprintf("  Present Loc: %O\n", environment(this_player())));
write("  Arrive Msg: " + arrive_msg + "\n");
write("  Depart Msg: " + depart_msg + "\n");
if(!victum){
 write("  Unable to get last joined info.\n");
} else {
 write("  Last Joined: " + victum->query_cap_name() + "\n");
 write(sprintf("     Which was at: %O\n",environment(victum)));
}
  return 1;
}

do_quit(){
  this_object()->move(environment(this_player()));
  this_player()->force_me("quit");
  say("A wand falls on the floor, and disappears in a puff of dark red smoke.\n");
  destruct(this_object());
  return 1;
}
drop(){
   return 0;
}

int do_goto(string dest){
  object loc;
  loc = 0;

if(!dest){
   write("You wand wonders where you want to go.\n");
   return 1;
}
 if(file_size(RESOLVE_PATH(dest)) > 0 || file_size(RESOLVE_PATH(dest + ".c")) > 0 ){
   loc = load_object(RESOLVE_PATH(dest));
   say(depart_msg + "\n");
   this_player()->move(loc);
   say(arrive_msg + "\n");
   write("You wand glows.\n");
   this_player()->force_me("look");
return 1;
}
write("Your wand can't find that destination.\n");
return 1;
}

