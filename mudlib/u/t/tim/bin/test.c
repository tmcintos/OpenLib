#include <cmdline.h>

#define WIDTH 80
void short_format(string* files, string dir, int optc, int optF);
void create();

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

      printstr = sprintf("%s%s%"+len+"' 's", printstr, files[j], "");
    }
    write(printstr +"\n");
  }
}

void
create()
{
}
