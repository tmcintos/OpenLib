//exa.c
//written by Lol on 10/15/95

#include <command.h>

int main(string str) {
object ob;
  if(!str) {
    notify_fail("Examine what?\n");
    return 0;
   }
  if(!present(str,environment(this_player())) && !present(str,this_player())) {
  environment(this_player())->exa_item(str);   /*  calls exa_item from room.c  */
   return 1; }
  if(ob=present(str,environment(this_player())) || ob=present(str,this_player())) {
    ob->long();
    return 1;
}
else {
}
}

void help() {
  write("this helps\n");
  return 1;
}
