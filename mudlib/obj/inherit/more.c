/*  -*- LPC -*-  */
// more.c by Tim
//
// CAVEATS:
//   Hitting 'h' repeatedly scrolls back...need to keep track of prev_line
//   as well as curr_line to do this right.

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
#define display(x) receive_message("system", x);

/*
 * Globals
 */
private int slen;               // screen length
private int swid;               // screen width
private int curr_line;          // next line to be displayed
private int lines;              // number of lines in file or array
private mixed file;             // file name or array of lines
private function callback;      // callback function
/*
 * Prototypes
 */
void receive_message(string msg_class, string msg);
mixed get_env(string var);

varargs int more(string filename, function callback);
private void more_file(string input);
private void more_array(string input);
private int more_show_help();

/*
 * more() call this with an argument that is either an array of lines
 *        to be displayed to the user (not newline terminated), or a filename
 *        of a file to display.  The optional callback function will be
 *        called after the text is displayed.
 */

varargs int
more(mixed file_or_array, function call_back)
{
  file = file_or_array;
  curr_line = 1;
  slen = get_env("LENGTH");
  swid = get_env("WIDTH");
  callback = call_back;

  if( !slen ) slen = 80;
  if( !swid ) swid = 80;

  if( arrayp(file) ) {
    file = map(file, (: $1 + "\n" :));
    lines = sizeof(file);
    more_array("X");
  } else {
    lines = file_length(file);
    more_file("X");
  }
  return 1;
}

/*
 * more_file()
 *   curr_line is always the 1st line of the next page to print
 */

private void
more_file(string input)
{
  int itmp;
  string text;

  switch(input[0]) {
  case ' ': case 0:                      // show NEXT page
//    display("\n");                       // need a newline
  case 'X' :                             // for starting; prints no newline
    break;
  case 'h' :                             // show HELP
    display("\n");                       // need a newline
    curr_line -= slen - 1;               // back to top of this page

    if(curr_line < 1)                       // we're on the 1st page already
      curr_line = 1;

    itmp = more_show_help() + 1;            // number of lines displayed
    itmp = slen - itmp;                     // fill the user's screen
    for(int i = 0; i < itmp; i++) display("\n");

    display(NOTIFY_CONT);

    input_to("more_file");
    return;
  case 's' :                             // SKIP FORWARD one page
    display("\n");                       // need a newline
    display("Skipping 1 page...\n");
    curr_line += slen - 1;
    break;
  case 'u' :
  case 'b' :                             // BACK one page
    display("\n");                       // need a newline
    curr_line -= 2 * (slen - 1);
    break;
  case 'q' :                             // Only way out
    display("\n");                       // need a newline
    evaluate(callback);
    return;
  default :
    input_to("more_file");
    return;
  }  /* switch(input) */

  /*
   * read the next screenfull
   */

  if(curr_line < 1)                      // we're on the 1st page already
    curr_line = 1;

  if(text = read_file(file, curr_line, slen - 1)) {
    display(text);
    curr_line += slen - 1;
  }

  /*
   * End of file encountered, but some text was read
   */
  if(text) {
    if(!read_file(file, curr_line, 1)) {
      itmp = slen - sizeof(explode(text, "\n"));

      if( get_env("MORE_EXIT_END") ) {
	evaluate(callback);
	return;
      }

      for(int i = 1; i < itmp; i++)
	display("~\n");
      display(NOTIFY_END);
    } else {
      display(sprintf("%s (%d%%) %s", NOTIFY_MORE,
		      (curr_line-1) * 100 / lines, NOTIFY_INST));
    }
  } else {
    // curr_line is past end of file, redraw screen
    return more_file("X");
  }
  input_to("more_file");
}

/*
 * more_array()
 */

private void
more_array(string input)
{
  int itmp, start_line;
  string text;

  start_line = curr_line;

  switch(input[0]) {
  case ' ': case 0:                      // show NEXT page
//    display("\n");                       // need a newline
  case 'X' :                             // for starting; prints no newline
    break;
  case 'h' :                             // show HELP
    display("\n");                       // need a newline
//    for(int i = 0; i < slen - 1; i++) {  // back to top of this page
//      string tmp;
//      tmp = lines[--curr_line - 1];
//      if(curr_line < 1) break;                // if we hit the top of page
//      i += (strlen(tmp) - 1) / swid;    // account for wrap-around lines
//    }
//
//    if(curr_line < 1)                   // we're on the 1st page already
    curr_line = start_line;               // top of this page

    itmp = more_show_help() + 1;          // number of lines displayed
    itmp = slen - itmp;                     // fill the user's screen
    for(int i = 0; i < itmp; i++) display("\n");

    display(NOTIFY_CONT);

    input_to("more_array");
    return;
  case 'q' :                             // only way out
    display("\n");                       // need a newline
    evaluate(callback);
    return;
  case 's' :                             // SKIP FORWARD one page
    display("\n");                       // need a newline
    display("Skipping 1 page...\n");
    for(int i = 0; i < slen - 1; i++) {
      string tmp;
      tmp = file[curr_line++ - 1];
      i += (strlen(tmp) - 1) / swid;   // account for wrap-around lines
      if(curr_line == lines + 1) {           // end of array
	curr_line = start_line;              // start where we were
	break;
      }
    }
    break;
  case 'u' :
  case 'b' :                                 // BACK one page
    display("\n");                           // need a newline
    for(int i = 0; i < 2*(slen - 1); i++) { // back to top of previous page
      string tmp;
      if(curr_line < 2) break;               // if we hit the top of document
      tmp = file[--curr_line - 1];
      i += (strlen(tmp) - 1) / swid;   // account for wrap-around lines
    }

    if(curr_line < 1)                        // we're on the 1st page already
      curr_line = 1;
    
    break;
  default :
    input_to("more_array");
    return;
  }  /* switch(input) */

  /*
   * display next page
   */
  for(int i = 0; i < slen - 1; i++) {
    if(curr_line > lines) {                   // past the end of array
      if( get_env("MORE_EXIT_END") ) {
	evaluate(callback);
	return;
      }
      display("~\n");
    } else {
      text = file[curr_line++ - 1];
      display(text);
      i += (strlen(text) - 1) / swid;  // account for wrap-around lines
    }
  }

  if(curr_line > lines) {                     // past the end of array
    curr_line = start_line;                  // reset curr_line
    display(NOTIFY_END);
  } else {
    display(sprintf("%s (%d%%) %s",
		    NOTIFY_MORE, 100 * (curr_line - 1) / lines, NOTIFY_INST));
  }
  input_to("more_array");
}

/*
 *  Help for more
 */

private int
more_show_help()
{
  display("Help for MORE:\n\n"
	  +"<spacebar>    Show next page\n"
	  +"u,b           Go back to the previous page\n"
	  +"h             Show this help\n"
	  +"s             Skip 1 page\n"
	  +"q             Quit\n");

  return (7);         // number of lines displayed
}
