/*  -*- LPC -*-  */
// drop.c:  drop something in your inventory
//   Originally by Tim 10/11/95
//

#include <command.h>

int
main(string what)
{
  object ob;

  if(!what || what == "")
    return notify_fail("Drop what??\n");

  ob = present(what, this_player());

  if(!ob)
    return notify_fail(sprintf("You don't have %s.\n", what));
  
  if(!ob->drop())
    return notify_fail("You can't drop that!\n");

// might want an ob->on_drop() here...

  ob->move(environment(this_player()));

  say(sprintf("%s drops %s.\n",
	      this_player()->query_cap_name(),
	      ob->short()));

  write("Ok.\n");
  return 1;
}
