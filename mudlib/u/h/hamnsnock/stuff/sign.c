#include <mudlib.h>

inherit OBJECT;

short(){
   return "a large sign";
}

long(){
    write("a large sign, what else could it be?\n");
    return 1;
}

id(str){
   return str == "sign";
}
