// exa.c:  written by Lol on 10/15/95
//
// 01.25.96:  Tim  Cleaned up a bit.  This should be merged in with 'look'

#include <command.h>

int main(string str) {
  object ob;

  if(!str) {
    notify_fail("Examine what?\n");
    return 0;
  } else if(!present(str, environment(this_player())) &&
	    !present(str, this_player())) {
    /*  calls exa_item from room.c  */
    environment(this_player())->exa_item(str);
  } else if(ob=present(str, environment(this_player())) ||
	    ob=present(str, this_player())) {
    ob->long();
  } else {
    printf("There's no %s around here.\n", str);
  }

  return 1;
}

string help()
{
  return "";
}
