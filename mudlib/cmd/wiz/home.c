// home.c
// -=Painfully coded by resident idiot Kyricc=-
//
// 10/18/95  Tim:  changed the find_object() to load_object() which also
//                 acts like find_object() if the obj is already loaded.
//                 Took out the line with the new() so we're not cloning a
//                 workroom (this was dumb mistake on my part).

#include <command.h>

int
main (string arg)
{
  string workroom, name;
  object me, ob;

  me = this_player();
  name = me->query_cap_name();

  workroom = RESOLVE_PATH("~/workroom");

  ob = load_object(workroom);

  if(!ob)
    return notify_fail("You do not have a workroom!\n");

  tell_room(environment(this_player()),
	    name + " hears his mother calling and runs home to mommy!\n",
	    ({this_player()}));

  me -> move(ob);
  me -> force_me("look");
  return 1;
}
