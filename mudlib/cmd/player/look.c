// look.c:  Look at room / item;  Tim McIntosh (astasia@iastate.edu)

#include <command.h>

int
main(string str)
{
  int i;
  object *obs, ob, me, room;

  me = this_player();
  room = environment(me);

  if(!str) {
    room->long();

    foreach(ob in all_inventory(room)) {
      string tmp;

      tmp = (string)ob->short();
      if(ob != me)
	write(tmp +"\n");
    }

    return 1;
  }

  if(sscanf(str, "at %s", str) != 1 ||
     (!(ob = present(str, room)) && !(ob = present(str, this_player()))))
    return notify_fail("Look AT something??\n");

  ob->long();
  
  return 1;
}
