#include <mudlib.h>

inherit MONSTER "/human";
object ob;

reset(arg)
{
  if(arg)
    return;
}

init(){
    add_action("do_conjure" , "conjure");
    add_action("do_read" , "read");
}

long(){
    write("You look at Shanu.  She is a retired mage.  She can only remember how to conjurer up little fun items, nothing more, nothing less.  She has a sign that she carries with her with a list of the items she can conjure.  Perhapes you should read it.\n");
    return 1;
}

create(){
   ::create();
set_name("shanu");
}

short(){
  return "shanu the retired mage";
}

id(str) {
   return str == "shanu";
}

do_read(string str){
if(!str){
    write("Read what?\n");
return 1;
}
if(str == "sign"){
   cat("/u/h/hamnsnock/stuff/shanu.txt");
   return 1;
}
   write("Shanu says:  I don't have that to read.\n");
   return 1;
}

do_conjure(string num){
if(!num){
   write("Shanu says:  Perhapes you should read my sign?\n");
   return 1;
}
if(num == "1"){
    say("Shanu look sat " + this_player()->query_cap_name() + " and waves her hands.  A magical button appears, which she then puts on the ground infront of her.\n");
    tell_object(this_player(), "Shanu waves her hands and a magical button appears.  She then puts it on the ground in front of her.\n");
    ob = clone_object("/u/h/hamnsnock/stuff/mbutton.c");
    ob->move(environment(this_object()));
    return 1;
}
write("Shanu says:  Perhapes you should read my sign?\n");
return 1;
}

