#include <command.h>
#include <daemons.h>

int
main(string str)
{
  NEWS_D->create_group(str);
  return 1;
}
