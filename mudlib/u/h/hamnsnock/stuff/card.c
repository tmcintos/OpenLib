#include <mudlib.h>

inherit OBJECT;

short(){
   return "a business card";
}

long(){
    write("This is Hamnsnock's Wizard Business Card\n");
    write("    He can be reached at: (MudMail) Hamnsnock@Dysfunctional\n");
    write("                          (EMail)   clintw@AZStarNet.COM\n");
    return 1;
}

id(str){
   return str == "card";
}
