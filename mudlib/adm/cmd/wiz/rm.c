/*  -*- LPC -*-  */
#include <command.h>
#include <cmdline.h>

int
_main(string file, int argc, string *argv, string *flags)
{
  int i, verbose;
  string *filez, path;

  if(!file)
    return notify_fail("usage: rm [-v] <file1 ... >\n");

  verbose = flag(flags, "v");

// take out verb and flags from list of words
  filez = filter_array(argv - ({ argv[0] }),
		       function(string word) {
			 if(word[0] == '-') return 0;
			 else return 1;
		       }
);

  foreach(file in filez) {
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
