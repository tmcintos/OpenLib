/*  -*- LPC -*-  */
// more.c by Tim
//
// CAVEATS:
//   Hitting 'h' repeatedly scrolls back...need to keep track of prev_line
//   as well as curr_line to do this right.

// Temporary!!
#define CHUNK (this_object()->get_env("LENGTH") ? \
	       this_object()->get_env("LENGTH") : 24)
#define WIDTH (this_object()->get_env("WIDTH") ? \
	       this_object()->get_env("WIDTH") : 80)

/*
 * Notification messages
 */
#define NOTIFY_END  "%^INVERSE%^(END)%^RESET%^ [hit 'q'<return> to continue]"
#define NOTIFY_MORE "%^INVERSE%^--MORE--%^RESET%^"
#define NOTIFY_INST " [<return> for more, 'h' for help]"
#define NOTIFY_CONT "%^INVERSE%^Hit <return> to Continue%^RESET%^"

/*
 * Used to properly display text to user
 */
#define display(x) message("system", x, this_object())

/*
 * Prototypes
 */
varargs int more(string filename, function callback);
void more_file(string input, string filename, int curr_line,
	   int screen_len, function callback);
void more_array(string input, string *lines, int curr_line, int size,
	   int screen_len, int screen_wid, function callback);
int more_show_help();

/*
 * more() call this with an argument that is either an array of lines
 *        to be displayed to the user (not newline terminated), or a filename
 *        of a file to display.  The optional callback function will be
 *        called after the text is displayed.
 */

varargs
int
more(mixed fileORarray, function callback)
{
  int total;

  if(arrayp(fileORarray)) {
    for(int i = 0; i < sizeof(fileORarray); i++)
      fileORarray[i] += "\n";
    more_array("X", fileORarray, 1, sizeof(fileORarray), CHUNK,
	       WIDTH, callback);
  } else {
    more_file("X", fileORarray, 1, CHUNK, callback);
  }

  return 1;
}

/*
 * more_file()
 *   curr_line is always the 1st line of the next page to print
 */

void
more_file(string input, string filename, int curr_line, int sl, function cb)
{
  int itmp, percent;
  string text;

  switch(input[0]) {
  case ' ': case 0:                      // show NEXT page
//    display("\n");                       // need a newline
  case 'X' :                             // for starting; prints no newline
    break;
  case 'h' :                             // show HELP
    display("\n");                       // need a newline
    curr_line -= sl - 1;         // back to top of this page

    if(curr_line < 1)                    // we're on the 1st page already
      curr_line = 1;

    itmp = this_object()->more_show_help() + 1;   // number of lines displayed
    itmp = sl - itmp;                     // fill the user's screen
    for(int i = 0; i < itmp; i++) display("\n");

    display(NOTIFY_CONT);

    input_to("more_file", filename, curr_line, sl, cb);
    return;
  case 's' :                             // SKIP FORWARD one page
    display("\n");                       // need a newline
    display("Skipping 1 page...\n");
    curr_line += sl - 1;
    break;
  case 'u' :
  case 'b' :                             // BACK one page
    display("\n");                       // need a newline
    curr_line -= 2 * (sl-1);
    break;
  case 'q' :                             // Only way out
    display("\n");                       // need a newline
    evaluate(cb);
    return;
  default :
    input_to("more_file", filename, curr_line, sl, cb);
    return;
  }  /* switch(input) */

  /*
   * read the next screenfull
   */

  if(curr_line < 1)                      // we're on the 1st page already
    curr_line = 1;

  if(text = read_file(filename, curr_line, sl-1)) {
    display(text);
    curr_line += sl - 1;
  }

  /*
   * End of file encountered, but some text was read
   */
  if(text) {
    if(!read_file(filename, curr_line, 1)) {
      itmp = sl - sizeof(explode(text, "\n"));

      if( this_object()->get_env("MORE_EXIT_END") ) {
	evaluate(cb);
	return;
      }

      for(int i = 1; i < itmp; i++)
	display("~\n");
      display(NOTIFY_END);
    } else {
      display(sprintf("%s (line: %d) %s",
		      NOTIFY_MORE, curr_line, NOTIFY_INST));
    }
  } else {
    // curr_line is past end of file, redraw screen
    return
      more_file("X", filename, curr_line - (sl - 1), sl, cb);
  }

  input_to("more_file", filename, curr_line, sl, cb);
}

/*
 * more_array()
 */

void
more_array(string input, string *lines, int curr_line, int size,
	   int sl, int sw, function cb)
{
  int itmp, percent, start_line;
  string text;

  start_line = curr_line;

  switch(input[0]) {
  case ' ': case 0:                      // show NEXT page
//    display("\n");                       // need a newline
  case 'X' :                             // for starting; prints no newline
    break;
  case 'h' :                             // show HELP
    display("\n");                       // need a newline
//    for(int i = 0; i < sl-1; i++) {  // back to top of this page
//      string tmp;
//      tmp = lines[--curr_line - 1];
//      if(curr_line < 1) break;                // if we hit the top of page
//      i += (strlen(tmp) - 1) / sw;    // account for wrap-around lines
//    }
//
//    if(curr_line < 1)                        // we're on the 1st page already
      curr_line = start_line;                     // top of this page

    itmp = this_object()->more_show_help() + 1;   // number of lines displayed
    itmp = sl - itmp;                     // fill the user's screen
    for(int i = 0; i < itmp; i++) display("\n");

    display(NOTIFY_CONT);

    input_to("more_array", lines, curr_line, size, sl, sw, cb);
    return;
  case 'q' :                             // only way out
    display("\n");                       // need a newline
    evaluate(cb);
    return;
  case 's' :                             // SKIP FORWARD one page
    display("\n");                       // need a newline
    display("Skipping 1 page...\n");
    for(int i = 0; i < sl-1; i++) {
      string tmp;
      tmp = lines[curr_line++ - 1];
      i += (strlen(tmp) - 1) / sw;   // account for wrap-around lines
      if(curr_line == size+1) {              // end of array
	curr_line = start_line;              // start where we were
	break;
      }
    }
    break;
  case 'u' :
  case 'b' :                                 // BACK one page
    display("\n");                           // need a newline
    for(int i = 0; i < 2*(sl-1); i++) { // back to top of previous page
      string tmp;
      if(curr_line < 2) break;               // if we hit the top of document
      tmp = lines[--curr_line - 1];
      i += (strlen(tmp) - 1) / sw;   // account for wrap-around lines
    }

    if(curr_line < 1)                        // we're on the 1st page already
      curr_line = 1;
    
    break;
  default :
    input_to("more_array", lines, curr_line, size, sl, sw, cb);
    return;
  }  /* switch(input) */

  /*
   * display next page
   */
  for(int i = 0; i < sl-1; i++) {
    if(curr_line > size) {                   // past the end of array
      if( this_object()->get_env("MORE_EXIT_END") ) {
	evaluate(cb);
	return;
      }
      display("~\n");
    } else {
      text = lines[curr_line++ - 1];
      display(text);
      i += (strlen(text) - 1) / sw;  // account for wrap-around lines
    }
  }

  if(curr_line > size) {                     // past the end of array
    curr_line = start_line;                  // reset curr_line
    display(NOTIFY_END);
  } else {
    display(sprintf("%s (%d%%) %s",
		    NOTIFY_MORE, (100 * curr_line) / size, NOTIFY_INST));
  }

  input_to("more_array", lines, curr_line, size, sl, sw, cb);
}

/*
 *  Help for more
 */

int
more_show_help()
{
  int i;

  display("Help for MORE:\n\n"
	  +"<spacebar>    Show next page\n"
	  +"u,b           Go back to the previous page\n"
	  +"h             Show this help\n"
	  +"s             Skip 1 page\n"
	  +"q             Quit\n");

  return (7);         // number of lines displayed
}
