#include <command.h>

inherit WEAPON;

string message;
string color = "red";
string message = "I gotta kewl button!"
;
long(){
   write("You are looking at a magic button.  You can set the color and the message of the button by using two commands:\n     buttoncolor <color>\n     buttonmessage <message>\n");
   return 1;
}

reset(arg) {
    if (arg)
        return;
  color = "red";
  message = "I gotta kewl button";
}

init(){
   add_action("do_buttoncolor" , "buttoncolor");
   add_action("do_buttonmessage" , "buttonmessage");
}

do_buttoncolor(string arg){
  if(!arg){
   write("usage: buttoncolor <color>\n");
  return 1;
  }
  color = arg;
  write("Button:  Color Set!\n");
  return 1;
}

do_buttonmessage(string str){
  if(!str){
   write("usage: buttonmessage <message>\n");
  return 1;
  }
  message = str;
  write("Button:  Message Set!\n");
  return 1;
}

short(){
if(environment(this_object()))
{
if(interactive(environment()))
  return "a " + color + "button, with message: " +message;
}
 return "a magical button";
}

id(str) {
   return str == "button";
}

