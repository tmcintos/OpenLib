/*  -*- LPC -*-  */
// ls.c: 05/05/95:  Tim McIntosh (astasia@iastate.edu)
//                  Supports flags -alF (single or combos)
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

#include <cmdline.h>

// screen width
#define WIDTH 80

void do_ls(string dir, int optl, int opta, int optc, int optF);
void long_format(string* files);
void short_format(string* files, string dir, int optc, int optF);
int catch_dots(mixed file);

int
main(string *argv, string *argv2)
{
  int optc, optF, optl, opta;
  int i;

  optF = flag("F");  // F flag marks loaded objects and dirs
  opta = flag("a");  // a flag lists all files including dot files
  optl = flag("l");  // list using long format
  optc = (this_player()->query_term()=="ansi" ||
	  this_player()->query_term()=="vt100");

  // No args on command line case
  if(!sizeof(argv2)) {
    do_ls(0, optl, opta, optc, optF);
    return 1;
  }

  // I did it this way to avoid the extra newline when only 1 arg is given
  for(i = 0; i < sizeof(argv2)-1; i++) {
    do_ls(argv2[i], optl, opta, optc, optF);
    write("\n");
  }

  do_ls(argv2[i], optl, opta, optc, optF);

  return 1;
}

void
do_ls(string dir, int optl, int opta, int optc, int optF) {
  mixed* files;
  int is_dir;

  if(!dir)
    dir = RESOLVE_PATH(".");
  else
    dir = RESOLVE_PATH(dir);

  if(file_size(dir) == -2) {                  //  directory so keep going
    printf("%s:\n", dir);
    if(dir[strlen(dir)-1] != '/')
      dir += "/";
    is_dir = 1;
  } else if(file_size(dir) >= 0) {            //  single file then we're done
    mixed *tmp;

    if(optl) {
      tmp = stat(dir);
      printf("   %24-s   %6-d   %30-s\n", dir, tmp[0], ctime(tmp[1]));
    } else
      write(dir + "\n");
    return;
  }
    
  if(opta) {
    files = get_dir(dir + "*", (optl ? -1 : 0) );
  } else {
    files = get_dir(dir, (optl ? -1 : 0) );
    if(files) files = filter(files, (: catch_dots :));
  }

  if(!is_dir && !sizeof(files))
    return write("ls: no such file or directory.\n");

  if(optl)
    long_format(files);
  else
    short_format(files, dir, optc, optF);
}

void
long_format(mixed* files)
{
  foreach(mixed* file in files) {
    string tmp = ( (file[1] == -2) ? "<DIR>" : "" + file[1] );
    printf("   %18-s   %6-s   %30-s\n", file[0], tmp, ctime(file[2]) );
  }
}

void
short_format(string* files, string dir, int optc, int optF)
{
  int max_fname_len, columns, col_len;
  string spaces;
  string printstr = "";
  spaces = "                                                                 ";
    
  // get longest filename and setup columns
  for(int i = 0; i < sizeof(files); i++) {
    int itmp = 1 + strlen( files[i] );

    if(itmp > max_fname_len)
      max_fname_len = itmp;
  }
  max_fname_len += 2;

  if(!(columns = ((WIDTH * 100) / max_fname_len) / 100)) columns = 1;
  col_len = to_int(ceil(to_float(sizeof(files)) / to_float(columns)));

  // create columns
  for(int i = 0; i < col_len; i++) {
    printstr = "";

    // create each row
    for(int j = i; j < col_len * columns; j += col_len) {
      int len, tmp_is_dir = 0, tmp_is_loaded = 0;

      if(j >= sizeof(files)) continue;         // if past the end of array

      len = max_fname_len - strlen(files[j]);  // how many spaces we need

      // Directory and loaded obj marking
      if(optF || optc) {
	if(file_size(dir + files[j]) == -2)    // mark directories
	  tmp_is_dir = 1;
	else if(find_object(dir + files[j]))   // mark loaded objs
	  tmp_is_loaded = 1;

	// 'c' flag:  tack on the perty colors
	if(optc) {
	  if(tmp_is_dir)
	    files[j] = "%^L_BLUE%^" + files[j] +"%^RESET%^";
	  else if(tmp_is_loaded)
	    files[j] = "%^L_GREEN%^"+ files[j] +"%^RESET%^";
	  else if(files[j][<2..<1] == ".c")
	    files[j] = "%^L_RED%^"+ files[j] +"%^RESET%^";
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
      printstr = sprintf("%s%s%"+len+"' 's", printstr, files[j], "");
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
