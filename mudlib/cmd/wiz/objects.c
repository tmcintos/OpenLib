/*  -*- LPC -*-  */
#include <command.h>

int
main()
{
  mapping info = ([]);
  string* output = ({ "Objects Loaded:" });
  string file;
  int i;

  foreach(object ob in objects()) {
    if(sscanf(file_name(ob), "%s#%*d", file) != 2) {
      file = file_name(ob);
    }

    if(!info[file])
      info[file] = 1;
    else
      info[file]++;
  }

  foreach(file, i in info)
    output += ({ sprintf("%-50.50'.'s...%d", file, i) });

  this_player()->more(output);

  return 1;
}
