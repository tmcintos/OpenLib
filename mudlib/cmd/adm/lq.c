#include <command.h>

int
main()
{
  mapping Q;
  Q = "/obj/daemon/net/inetd"->dump_queue();

  write("Queued:\n");
  foreach(string mud, mixed *packet in Q) {
    printf("%s: %O\n", mud, packet);
  }
  return 1;
}
