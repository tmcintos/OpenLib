/*  -*- LPC -*-  */
// get.c:  get an object and place in your inventory
//   Originally by Tim 10/11/95
//

#include <command.h>

int
main(string what)
{
  string from, short;
  object ob, source;
  int err;

  if(!what || what == "")
    return notify_fail("Get what??\n");

  if(sscanf(what, "%s from %s", what, from) == 2) {
    source = present(from, this_player());
    if(!source)
      source = present(from, environment(this_player()));
    if(!source)
      return notify_fail(sprintf("Can't find %s here to get it from!\n",
				 from));
  } else {
    source = environment(this_player());
  }

  ob = present(what, source);

  if(!ob) {
    if(source)
      return notify_fail(sprintf("Can't find %s in %s.\n", what, from));
    else 
      return notify_fail(sprintf("Can't find %s here.\n", what));
  }

  if(!ob->get())
    return notify_fail("You can't get that!\n");

// Object weight stuff here...
// don't know if we'll want to do a ob->on_get() or just do all in init()

  short = ob->short();            // ob not guaranteed to exist after move

  if(err = ob->move(this_player()) < 1)
    return notify_fail("You fail.\n");      // add error detection later

  if(source && source != environment(this_player()))
    say(sprintf("%s gets %s from %s.\n",
		this_player()->query_cap_name(),
		short,
		source->short()));
  else
    say(sprintf("%s gets %s.\n",
		this_player()->query_cap_name(),
		short));

  write("Ok.\n");
  return 1;
}
