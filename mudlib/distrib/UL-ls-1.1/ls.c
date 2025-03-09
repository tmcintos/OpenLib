/*  -*- LPC -*-  */
// ls.c: 05/05/95:  Tim McIntosh (astasia@iastate.edu)
//                  Supports flags -aclF (single or combos)
//
// This file is part of the UltraLib distribution mudlib.
// It may be used and modified as long as this header remains intact.
// The UltraLib mudlib is available via FTP at knapp38.res.iastate.edu
//
//       08/26/95:  T.M   Added wildcard support
//       08/27/95:  T.M   Fixed so files are listed alphabetically in columns
//       10/10/95:  T.M   Fixed the -F flag (had indexing bug in it)
//       10/12/95:  T.M   Added -c flag for COLORizing ls (not with -l though)
//       10/30/95:  T.M   Broke into 2 funcs, now allows for unlimited command
//                        line arguments.
//       12.10.95:  T.M   Changed normal ls to use sprintf()
//       12.17.95:  T.M   Made output a little prettier
//       02.12.96:  T.M   Broke do_ls() into short_format() and long_format()
//       04.16.96:  T.M   Optimized some...
//       04.22.96   T.M   Fixed one line directory bug.
//       06.12.96   T.M   Updated w.r.t. security stuff--more cleanup when
//                        efuns are made.
//       07.03.96   T.M   Added coloring for .h and .o files

#include <cmdline.h>
#include <daemons.h>

int do_ls(string dir, int optl, int opta, int optc, int optd, int optF);
void long_print_file(string dir, mixed* file);
void short_format(string* files, string dir, int optc, int optF);
int catch_dots(mixed file);

int
main(string *argv, string *argv2)
{
  int optc, optF, optl, opta, optd;
  int i, ret;

  optF = flag("F");  // F flag marks loaded objects and dirs
  opta = flag("a");  // a flag lists all files including dot files
  optl = flag("l");  // list using long format
  optc = flag("c");  // color list
  optd = flag("d");  // force list of directory name only

  // No args on command line case
  if(!sizeof(argv2)) {
    do_ls(0, optl, opta, optc, optd, optF);
    return 1;
  }

  // I did it this way to avoid the extra newline when only 1 arg is given
  for(i = 0; i < sizeof(argv2)-1; i++) {
    ret = do_ls(argv2[i], optl, opta, optc, optd, optF) || ret;
    write("\n");
  }
  ret = do_ls(argv2[i], optl, opta, optc, optd, optF) || ret;

  notify_fail("");
  
  return ret;
}

int
do_ls(string dir, int optl, int opta, int optc, int optd, int optF) {
  mixed* files;
  int is_dir;

  if(!dir)
    dir = RESOLVE_PATH(".");
  else
    dir = RESOLVE_PATH(dir);

  if( file_size(dir) == -1 )
    return write(query_notify_fail() ? query_notify_fail() :
		 "ls: "+ dir +": no such file or directory.\n");

  if( !optd && file_size(dir) == -2) {        //  directory so keep going
    printf("%s:\n", dir);
    if(dir[<1] != '/')
      dir += "/";
    is_dir = 1;
  } else {            //  single file then we're done
    mixed*tmp;

    if(optl) {
      tmp = get_dir(dir, -1)[0];
      tmp[0] = dir;
      long_print_file("", tmp);
    } else
      write(dir + "\n");
    return 1;
  }
    
  if(opta) {
    files = get_dir(dir + "*", (optl ? -1 : 0) );
  } else {
    files = get_dir(dir, (optl ? -1 : 0) );
    if(files) files = filter(files, (: catch_dots :));
  }

  if(optl)
    map(files, (: long_print_file, dir :));
  else
    short_format(files, dir, optc, optF);

  return 1;
}

void
long_print_file(string dir, mixed* file)
{
  
  string ct = ctime(file[2]);
  printf("%:-10s %-10s %6s %s %s\n",
	 sprint_priv(SECURITY_D->get_file_protection(dir + file[0], 0)),
	 sprint_priv(SECURITY_D->get_file_protection(dir + file[0], 1)),
	 (file[1] == - 2 ? "<DIR>" : "" + file[1]),
	 ct[4..15] + ct[19..],
	 file[0]);
}

void
short_format(string* files, string dir, int optc, int optF)
{
  int width = this_player()->get_env("WIDTH") - 1;
  int max_fname_len, columns, col_len;

  if( !width ) width = 79;

  if( !sizeof(files) ) return;

  // get longest filename and setup columns
  max_fname_len = sort_array(map(files, (: strlen($1) :)), 1)[<1] + 2;

  if( !(columns = width / max_fname_len) ) columns = 1;
  col_len = sizeof(files) / columns + 1;

  // create columns
  for(int i = 0; i < col_len; i++) {
    string printstr = "";

    // create each row
    for(int j = i; j < col_len * columns; j += col_len) {
      int len, tmp_is_dir = 0, tmp_is_loaded = 0;

      if(j >= sizeof(files)) continue;         // if past the end of array

      len = max_fname_len - strlen(files[j]);  // how many spaces we need

      // Directory and loaded obj marking
      if(optF || optc) {
	// mark directories--checking for ".." is faster than resolving path
	if(file_size(dir + files[j]) == -2 || files[j] == "..")
	  tmp_is_dir = 1;
	else if(find_object(dir + files[j]))   // mark loaded objs
	  tmp_is_loaded = 1;

	// 'c' flag:  tack on the perty colors
	if(optc) {
	  if(tmp_is_dir)
	    files[j] = "%^L_BLUE%^" + files[j] +"%^RESET%^";
	  else if(tmp_is_loaded)
	    files[j] = "%^L_GREEN%^"+ files[j] +"%^RESET%^";
	  else {
	    switch(files[j][<2..<1]) {
	    case ".c":
	      files[j] = "%^L_RED%^"+ files[j] +"%^RESET%^";
	      break;
	    case ".h":
	      files[j] = "%^L_YELLOW%^" + files[j] +"%^RESET%^";
	      break;
	    case ".o":
	      files[j] = "%^L_CYAN%^" + files[j] + "%^RESET%^";
	      break;
	    }
	  }
	}

	// 'F' flag:  tack on symbols
	if(optF) {
	  if(tmp_is_dir)
	    files[j] += "/";
	  else if(tmp_is_loaded)
	    files[j] += "*";
	  else
	    files[j] += " ";	                  // this is a balance thing
	  len--;                                  // so is this
	}
      }
      printstr = sprintf("%s%s%*' 's", printstr, files[j], len, "");
    }
    write(printstr +"\n");
  }
}

int
catch_dots(mixed file)
{
  if(arrayp(file[0]))
    return file[0][0] != '.';
  else
    return file[0] != '.';
}

string
help()
{
  return @ENDHELP
usage:  ls [-alF] [filespec ...]

List files in current directory if no filespec given.
ENDHELP;
}
