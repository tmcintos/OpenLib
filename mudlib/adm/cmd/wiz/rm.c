/*  -*- LPC -*-  */
#include <command.h>
#include <cmdline.h>

int
_main(string file, int argc, string *argv, string *flags)
{
  int i, verbose;
  string *filez, path;

  if(!file)
    return notify_fail("usage: rm [-ov] <filename> (Use caution with -o!!!)\n");

  file = RESOLVE_PATH(file);
  path = file[0..strsrch(file, "/", -1)];

  verbose = flag(flags, "v");

  if(!flag(flags, "o")) {
    if(file_size(file) < 0)
      return notify_fail("rm: file does not exist.\n");
    if(verbose) write(file + "\n");
    rm(file);
  } else {                                      // get all matching files
    filez = (string *)get_dir(file);

    for(i=0;i < sizeof(filez);i++) {
      if(filez[i] == "..") continue;            // skip parent directory
      if(file_size(path + filez[i]) == -2) {
	write("rm: "+ path + filez[i] +" is a directory.\n");
	continue;
      }
      if(verbose) write(path + filez[i] + "\n");
      rm(path + filez[i]);
    }
  }
  return 1;
}
