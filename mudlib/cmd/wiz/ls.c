/*  -*- LPC -*-  */
// ls.c: 05/05/95:  Tim McIntosh (astasia@iastate.edu)
//                  Supports flags -alF (single or combos)
//       08/26/95:  T.M   Added wildcard support
//       08/27/95:  T.M   Fixed so files are listed alphabetically in columns
//       10/10/95:  T.M   Fixed the -F flag (had indexing bug in it)
//       10/12/95:  T.M   Added -c flag for COLORizing ls (not with -l though)
//       10/30/95:  T.M   Broke into 2 funcs, now allows for unlimited command
//                        line arguments.

#include <command.h>
#include <cmdline.h>

#define WIDTH 80

void do_ls(string dir, string* flags, int optc, int optF);

int
_main(string cmdline, int argc, string *argv, string *flags)
{
  string* filez;
  string file;
  int optc, optF;

  optc = (this_player()->query_term()=="ansi" || this_player()->query_term()=="vt102");
  optF = flag(flags, "F");

  if(!cmdline) {
    do_ls(0, flags, optc, optF);
    write("\n");
    return 1;
  }

  filez = filter_array(argv - ({argv[0]}),
		       function(string item) {
			 return item[0] != '-';
		       }
		       );
 
  foreach(file in filez) {
    do_ls(file, flags, optc, optF);
    write("\n");
  }

  return 1;
}

void
do_ls(string dir, string* flags, int optc, int optF) {
  string *files, printstr;
  string spaces;
  int max_fname_len, columns, col_len;
  int is_dir;
  int i, j;
  spaces = "                                                                 ";

  if(!dir)
    dir = RESOLVE_PATH(".");
  else
    dir = RESOLVE_PATH(dir);

  printf("%s:\n", dir);

  if(file_size(dir) == -2) {                  //  directory
    if(dir[strlen(dir)-1] != '/')
      dir += "/";
    is_dir = 1;
  } else if(file_size(dir) >= 0) {            //  single file
    mixed *tmp;
    if(flag(flags, "l")) {
      tmp = stat(dir);
      printf("   %24-s   %6-d   %30-s\n", dir, tmp[0], ctime(tmp[1]));
    } else
      write(dir + "\n");
    return;
  }
    
/* ----------------------------- Long Format ------------------------------ */
  if(flag(flags, "l")) {
    if(flag(flags, "a")) {
      files = get_dir(dir+"*", -1);
    } else {
      files = get_dir(dir, -1);
      if(files) files = filter_array(files, "catch_dots2", this_object());
    }
    
    if(!is_dir && sizeof(files) == 0)
      return write("ls: no such file or directory.\n");

    for(i=0;i < sizeof(files);i++) {
      string tmp;
      tmp = ((files[i][1] == -2) ? "<DIR>" : ""+files[i][1]);
	
      printf("   %18-s   %6-s   %30-s\n",
	     files[i][0], tmp, ctime(files[i][2]));
    }
    return;
  }   // END of Long Format


/* --------------------- Regular Format ----------------------------------- */
  printstr = "";
    
  if(!flag(flags, "a")) {
    files = get_dir(dir);
    if(files) files = filter_array(files, "catch_dots", this_object());
  } else {
    files = get_dir(dir+"*");
  }    

  if(!is_dir && sizeof(files) == 0)
    return write("ls: no such file or directory.\n");

// get longest filename and setup columns
  max_fname_len = 0;
  for(i=0;i < sizeof(files);i++) {
    int itmp;
    if((itmp = 1 + strlen(files[i])) > max_fname_len)
      max_fname_len = itmp;
  }
  max_fname_len += 2;

  if(!(columns = ((WIDTH * 100) / max_fname_len) / 100)) columns = 1;
  col_len = to_int(ceil(to_float(sizeof(files)) / to_float(columns)));

  for(i=0;i < col_len;i++) {                        // create columns
    printstr = "";
    for(j=i;j < col_len * columns;j += col_len) {   // create each row
      int len, tmp_is_dir = 0, tmp_is_loaded = 0;

      if(j >= sizeof(files)) continue;         // if past the end of array

      if(optF || optc) {
	if(file_size(dir+files[j]) == -2)    // mark directories
	  tmp_is_dir = 1;
	else if(find_object(dir+files[j]))   // mark loaded objs
	  tmp_is_loaded = 1;
      }

      len = max_fname_len - strlen(files[j]);  // how many spaces we need

      if(optc) {                      // 'c' flag:  tack on the perty colors
	if(tmp_is_dir)
	  files[j] = "%^L_BLUE%^" + files[j] +"%^RESET%^";
	else if(tmp_is_loaded)
	  files[j] = "%^L_GREEN%^"+ files[j] +"%^RESET%^";
      }

      if(optF) {                     // 'F' flag:  tack on symbols
	if(tmp_is_dir)
	  files[j] += "/";
	else if(tmp_is_loaded)
	  files[j] += "*";
	else
	  files[j] += " ";                         // this is a balance thing
	len -= 1;                                  // so is this :)
      }

      files[j] += spaces[0..len - 1];              // get the spacing right

      printstr += files[j];
    }
    write(printstr +"\n");
  }
  return;
}

int
catch_dots(string file)
{
  if(file[0] == '.') {
    return 0;
  }
  return 1;
}

int
catch_dots2(string *file)
{
  if(file[0][0] == '.') {
    return 0;
  }
  return 1;
}
