/*  -*- LPC -*-  */
// cp.c:  copy a file;  Tim McIntosh (astasia@iastate.edu)

#include <command.h>
#include <cmdline.h>

#define USAGE "cp [-o] <source> <destination>"

int
_main(string args, int argc, string *argv, string *flags)
{
  string source, dest, *tmp;

  if(!args || sscanf(args, "%s %s", source, dest) != 2)
    return notify_fail("usage: "+ USAGE +"\n");

  tmp = explode(source, "/");

  source = RESOLVE_PATH(source);
  dest = RESOLVE_PATH(dest);

  if(file_size(source) < 0)
    return notify_fail("cp: source file does not exist.\n");

  if(file_size(dest) == -2) {               // in case just gave dir name
    if(dest[strlen(dest)-1] != '/')         // make sure we get the / right...
      dest += "/";
    dest += tmp[sizeof(tmp) - 1];
  }

  if(!flag(flags, "o") && file_size(dest) != -1)
    return notify_fail("cp: destination file already exists.\n");
  else
    rm(dest);

  if(cp(source, dest) != 1)
    return notify_fail("cp: call to cp() failed.\n");

  return 1;
}
