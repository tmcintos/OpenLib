#include <cmdline.h>
#include <daemons.h>

int
main(string* argv1, string* argv2)
{
  NEWS_D->new_post(argv2[0], argv2[1], argv2[2],
		   this_player()->query_cap_name(), argv2[3]);
  return 1;
}
