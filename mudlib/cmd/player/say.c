#include <command.h>

int
main(string arg)
{
  if(!arg)
    return notify_fail("usage:  say <text>\n");

  say(capitalize(this_player()->query_name())
      + " says, \""+capitalize(arg)+"\"\n");

  write("You say, \""+arg+"\"\n");

  return 1;
}
