/*  -*- LPC -*-  */
// pwd.c:  Print working directory;  Tim McIntosh (astasia@iastate.edu)

#include <command.h>

int
main()
{
  write(this_player()->query_cwd() +"\n");
  return 1;
}
