#include <mudlib.h>

inherit MONSTER "/clay";

create(){
  ::create();
   set_name("Tit");
}

long(){
   write("A large tit\n");
   return 1;
}

short(){
   return "a mighty tit";
}

id(str){
   return str == "tit";
}
