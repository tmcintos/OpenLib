//  put.c: puts objs into things
//  coded by Kyricc
//  24 Feb 96

#include <command.h>

int
main (string what)
{
  string where, short;
  object ob, destination;
  int err;

  if(!what || what == "")
    return notify_fail("Put what where??\n");

  if(sscanf(what, "%s in %s", what, where) == 2) {
    destination = present(where, this_player());
    if(!destination)
    destination = present(where, environment(this_player()));
    if(!destination)
      return notify_fail(sprintf("Cant find %s here to put it into!\n",
                                 where));
      }

  ob = present(what, this_player());

  if(!ob)
    return notify_fail("You don't have that!\n");
  ob->move(destination);
  write("Ok.\n");
  return 1;
}
