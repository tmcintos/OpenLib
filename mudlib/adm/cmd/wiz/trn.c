#include <command.h>

int
main()
{
  mapping newsrc = ([ ]);
  string* lines;
  string file = user_cwd(this_player()->query_name()) + "/.newsrc";

  write("Getting your newsrc...\n");

  if( !file_exists(file) )
    write("You have no newsrc!  Using empty newsrc.\n");
  else
  {
    lines = explode(read_file(file), "\n");

    foreach(string line in lines)
    {
      string group, list;
      if( sscanf(line, "%s:%s", group, list) != 2 )
      {
	write("Corrupt line in newsrc file...continuing\n");
	continue;
      }
      newsrc[group] = list;
    }
  }

  return 1;
}
