#include <command.h>

main(string str)
{
  if(!str) str = "";
  write(to_string(atoi(str)) +"\n");
  return 1;
}
