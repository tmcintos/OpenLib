/*  -*- LPC -*-  */
// du.c:  Displays disk usage in Kilobytes for a given directory or file.
//
// Created by Tim 18 Jul 1996
//
#include <cmdline.h>

int du(string p, int silent);

int
main(string* argv1, string* argv2)
{
  string path;
  int size, opts = flag("s");

  if( !sizeof(argv2) )
    path = this_player()->query_cwd();
  else
    path = argv2[0];

  path = RESOLVE_PATH(path);

  switch( size = file_size(path) )
  {
  case -1:
    return notify_fail("du: No such file or directory.\n");
  case -2:
    if( path[<1] != '/' )
      path += "/";
    size = du(path, opts);
    if( !opts )
      return 1;
    break;
  }    

  message("system", sprintf("%-10i %s\n", size/1024, path), this_player());
  return 1;
}

// PRE: p is a directory, last char is '/'
// POST: returns number of bytes used by file hierarchy rooted at p
int
du(string p, int silent)
{
  mixed* dir_stats = get_dir(p, -1);
  mixed* directories = filter(dir_stats, (: $1[1] < 0 :));
  int ret;

  foreach(mixed* file in dir_stats - directories)
    ret += file[1];

  foreach(mixed* dir in directories)
    ret += du(p + dir[0] + "/", silent);

  if( !silent )
    message("system", sprintf("%-10i %s\n", ret/1024, p), this_player());

  return ret;
}
