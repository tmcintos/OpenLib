#include <command.h>
#include <daemons.h>

int
main(string arg)
{
  string name;
  object me = this_player();

  name = (string)me->query_cap_name();
  say(name + " leaves this reality.\n");
  write("Saving " + name +"....\n");
  me->save_player();
  write("Bye.\n");
  me->remove();
  return 1;
}
