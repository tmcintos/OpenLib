/*  -*- LPC -*-  */
#include <cmdline.h>

int
main(string *argv, string *argv2)
{
  int verbose;

  if(!sizeof(argv2))
    return notify_fail("usage: rm [-v] <file1 ... >\n");

  verbose = flag("v");

  foreach(string file in argv2) {
    file = RESOLVE_PATH(file);
    
    switch(file_size(file)) {
    case -1:
      printf("rm: %s does not exist.\n", file);
      break;
    case -2:
      printf("rm: %s is a directory.\n", file);
      break;
    }
    if(verbose) write(file + "\n");
    rm(file);
  }

  return 1;
}
