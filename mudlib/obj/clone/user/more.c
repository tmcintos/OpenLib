/*  -*- LPC -*-  */
// more.c

// Temporary!!
#define CHUNK 24
#define WIDTH 80

/* Prototypes */
int more(string filename);
void more2(string input, string filename, int curr_line,
	   int screen_len, int screen_wid);
void more3(string input, string *lines, int curr_line, int size,
	   int screen_len, int screen_wid);

int
more(mixed fileORarray)
{
  int total;

  if(arrayp(fileORarray)) {
    int i, size = sizeof(fileORarray);

    for(i=0; i<size; i++)
      fileORarray[i] += "\n";

    more3("X", fileORarray, 1, size, CHUNK, WIDTH);
  } else {
    more2("X", fileORarray, 1, CHUNK, WIDTH);
  }

  return 1;
}

//
// more2()
//   curr_line is always the 1st line of the next page to print
//

void
more2(string input, string filename, int curr_line,
	   int screen_len, int screen_wid)
{
  int i;
  int itmp, percent;
  string text;

  switch(input[0]) {
  case ' ' :                             // show NEXT page
    tell_object(this_object(), "\n");                         // need a newline
    break;
  case 'X' :                             // for starting; prints no newline
    break;
  case 'h' :                             // show HELP
    tell_object(this_object(), "\n");                         // need a newline
    curr_line -= screen_len-1;             // back to top of this page

    if(curr_line < 1)                         // we're on the 1st page already
      curr_line = 1;

    itmp = this_object()->more_show_help() + 1;   // number of lines displayed
    itmp = screen_len - itmp;                     // fill the user's screen
    for(i=0;i < itmp;i++) tell_object(this_object(), "\n");

    tell_object(this_object(), "%^INVERSE%^Hit Spacebar to Continue%^RESET%^");

    get_char("more2", 1, filename, curr_line, screen_len, screen_wid);
    return;
  case 'q' :
    tell_object(this_object(), "\n");                         // need a newline
    return;
  case 's' :                             // SKIP FORWARD one page
    tell_object(this_object(), "\n");                         // need a newline
    tell_object(this_object(), "Skipping 1 page...\n");
    curr_line += screen_len-1;
    break;
  case 'u' :
  case 'b' :                             // BACK one page
    tell_object(this_object(), "\n");                         // need a newline
    curr_line -= 2 * (screen_len-1);

    if(curr_line < 1) {                  // we're on the 1st page already
      curr_line = 1;
      break;                             // we're done
    }
    break;
  default :
    get_char("more2", 1, filename, curr_line, screen_len, screen_wid);
    return;
  }  /* switch(input) */

// read the next screenfull
  if(text = read_file(filename, curr_line, screen_len-1)) {
    tell_object(this_object(), text);
    curr_line += screen_len - 1;
  }

// End of file encountered
  if(!read_file(filename, curr_line, 1)) {
    tell_object(this_object(), "EOF\n");
    return;
  }

  tell_object(this_object(), "%^INVERSE%^--More--(line: " + curr_line +
	")%^RESET%^  [<spacebar> for more, 'h' for help] ");

  get_char("more2", 1, filename, curr_line, screen_len, screen_wid);
  return;
}

int
more_show_help()
{
  int i;

  tell_object(this_object(), "Help for MORE:\n\n"
	+"<spacebar>    Show next page\n"
	+"u,b           Go back to the previous page\n"
	+"h             Show this help\n"
	+"s             Skip 1 page\n"
	+"q             Quit\n");

  return (7);         // number of lines displayed
}

void
more3(string input, string *lines, int curr_line, int size,
	   int screen_len, int screen_wid)
{
  int i;
  int itmp, percent;
  string text;

  switch(input[0]) {
  case ' ' :                             // show NEXT page
    tell_object(this_object(), "\n");                         // need a newline
    break;
  case 'X' :                             // for starting; prints no newline
    break;
  case 'h' :                             // show HELP
    tell_object(this_object(), "\n");                         // need a newline
    for(i=0;i < screen_len-1;i++) {      // back to top of this page
      string tmp;
      tmp = lines[--curr_line - 1];
      if(curr_line < 1) break;                // if we hit the top of document
      i += (strlen(tmp) - 1) / screen_wid;    // account for wrap-around lines
    }

    if(curr_line < 1)                         // we're on the 1st page already
      curr_line = 1;

    itmp = this_object()->more_show_help() + 1;   // number of lines displayed
    itmp = screen_len - itmp;                     // fill the user's screen
    for(i=0;i < itmp;i++) tell_object(this_object(), "\n");

    tell_object(this_object(), "%^INVERSE%^Hit Spacebar to Continue%^RESET%^");

    get_char("more3", 1, lines, curr_line, size, screen_len, screen_wid);
    return;
  case 'q' :
    tell_object(this_object(), "\n");                         // need a newline
    return;
  case 's' :                             // SKIP FORWARD one page
    tell_object(this_object(), "\n");                         // need a newline
    tell_object(this_object(), "Skipping 1 page...\n");
    for(i=0;i < screen_len-1;i++) {
      string tmp;
      tmp = lines[curr_line++ - 1];
      i += (strlen(tmp) - 1) / screen_wid;    // account for wrap-around lines
      if(curr_line == size+1) {
	tell_object(this_object(), "EOF\n");
	return;                          // we're done
      }
    }
    break;
  case 'u' :
  case 'b' :                             // BACK one page
    tell_object(this_object(), "\n");                         // need a newline
    for(i=0;i < 2*(screen_len-1);i++) {  // back to top of previous page
      string tmp;
      if(curr_line < 2) break;           // if we hit the top of document
      tmp = lines[--curr_line - 1];
      i += (strlen(tmp) - 1) / screen_wid;    // account for wrap-around lines
    }

    if(curr_line < 1) {                 // we're on the 1st page already
      curr_line = 1;
      break;                             // we're done
    }
    break;
  default :
    get_char("more3", 1, lines, curr_line, size, screen_len, screen_wid);
    return;
  }  /* switch(input) */

// display next page
  for(i=0;i < screen_len-1;i++) {
    text = lines[curr_line++ - 1];
    tell_object(this_object(), text);
    i += (strlen(text) - 1) / screen_wid;    // account for wrap-around lines
    if(curr_line == size+1) {
      tell_object(this_object(), "EOF\n");
      return;                          // we're done
    }
  }

  tell_object(this_object(), "%^INVERSE%^--More--(" + (100 * curr_line) / size +
	"%)%^RESET%^  [<spacebar> for more, 'h' for help] ");

  get_char("more3", 1, lines, curr_line, size, screen_len, screen_wid);
  return;
}
