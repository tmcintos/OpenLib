#include <command.h>

int
main()
{
  object *inv, ob;
  int vis_ob_found = 0;

  inv = all_inventory(this_player());

  write("You are carrying:\n");

  foreach(ob in inv) {
    string sh = ob->short();
    if(sh) {
      vis_ob_found = 1;
      write(capitalize(sh) + "\n");
    }
  }

  if(!vis_ob_found)
    write("(nothing)\n");

  return 1;
}
