#include <command.h>
#include <element.h>

int
main()
{
  element* Q;
  Q = "/obj/daemon/net/inetd"->dump_queue();

  write("Queued:\n");
  foreach(mixed mud in Q) {
    printf("%s: %O\n", ((element) mud)->key, ((element) mud)->val);
  }

  return 1;
}
