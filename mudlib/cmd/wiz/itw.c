#include <command.h>

inherit INHERIT_DIR "/inttostr";

main(string str)
{
  if(!str) str = "";
  write(int_to_word(atoi(str)) +"\n");
  return 1;
}
