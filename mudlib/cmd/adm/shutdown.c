/*  -*- LPC -*-  */
#include <command.h>

int
main(string arg)
{
  write("Shutting down imediately...\n");
  log_file("shutdown",
             sprintf("mud shut down by %s\n", this_player()->query_cap_name()));
  shutdown(0);
  return 1;
}
