// look.c:  Look at room / item;  Tim McIntosh (astasia@iastate.edu)
//   10/16/95   Tim:  capitalizes room inventory now
//   10/18/95   Tim:  added object identifier line
//   10/25/95   Tim:  added filename identifier for ob's w/no short for wizzes

#include <command.h>

int
main(string str)
{
  int i;
  object *obs, ob, me, room;

  me = this_player();
  room = environment(me);

  if(!str) {
// put a check for wizards here later
    printf("[%s]\n", file_name(room));

    room->long();

    foreach(ob in all_inventory(room)) {
      string tmp;

      tmp = (string)ob->short();

      if(ob != me) {
	if(tmp)
	  write(capitalize(tmp) +"\n");
	else if(1)                            // Wiz check here
	  printf("Invis Object: %s\n", file_name(ob));
      }
    }

    return 1;
  }

  if(sscanf(str, "at %s", str) != 1 ||
     (!(ob = present(str, room)) && !(ob = present(str, this_player()))))
    return notify_fail("Look AT something??\n");

  ob->long();
  
  return 1;
}
