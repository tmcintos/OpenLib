#include <command.h>
#include <daemons.h>

int
main(string str)
{
  string chan, msg;

  if(sscanf(str, "%s:%s", chan, msg) != 2)
    return notify_fail("chat <channel>:<message>\n");

  if(!CHAT_D->cmdChannel(chan, msg))
    return notify_fail("chat on channel <"+chan+"> failed\n");

  return 1;
}
