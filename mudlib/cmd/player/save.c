#include <command.h>

int
main()
{
  if(this_player()->save_player())
    printf("save: user %s saved.\n", this_player()->query_cap_name());
  else
    printf("save: unable to save %s!! Please report this to a wizard.\n");

  return 1;
}
