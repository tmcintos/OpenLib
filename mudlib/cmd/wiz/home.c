// -=Painfully coded by resident idiot Kyricc=-

#include <command.h>

int
main (string arg)
{
  string workroom, name;
  object me, ob;

  me = this_player();
  name = me->query_cap_name();

  workroom = "~/workroom";

  ob = find_object(RESOLVE_PATH(workroom));
  if(!ob) ob = new(RESOLVE_PATH(workroom));
  if(!ob) return notify_fail("You do not have a workroom!\n");
  tell_room(environment(this_player()), name + " hears his mother calling and runs home to mommy!\n", ({this_player()}));
  me -> move(ob);
  me -> force_me("look");
  return 1;
}
