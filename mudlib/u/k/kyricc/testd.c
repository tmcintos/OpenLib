#include <daemons.h>
#include <net/config.h>
#include <net/network.h>

inherit TCP_SERVER;

mapping input;

void create();
void setup();

void
create()
{
  server::create();
  call_out("setup", 1);
  input = ([]);
}

void
setup()
{
  eventListenSocket(5555, MUD);
}

void
eventRead(int fd, mixed val)
{
  message("system", dump_variable(val), find_player("kyricc"));
}
