/*  -*- LPC -*-  */
#include <command.h>

int
main()
{
  string blurb = "";
  object *inv, ob;

  inv = all_inventory(this_player());

  foreach(ob in inv) {
    string sh = ob->short();

    if(sh) {
      blurb += sprintf("%s\n", capitalize(sh));
    } else if(1) {                                /* WIZ check here */
      blurb += sprintf("Invisible object: %s\n", file_name(ob));
    }
  }

  if(blurb == "")
    write("You are empty handed\n");
  else
    write("You are carrying:\n" + blurb);

  return 1;
}
