// editor.c: editor for entering text
// Written 12.10.95 by Tim (astasia@iastate.edu)
//
// This file is part of the UltraLib distribution mudlib.
// It may be used and modified as long as this header remains intact.
// The UltraLib mudlib is available via FTP at knapp38.res.iastate.edu

void enter_text(function call_func, function abort_func, mixed* args ... );
void get_text(string input, int count, function call_func,
	      function abort_func, mixed* args);

string* lines;

void
create()
{
  lines = ({});
}

void
enter_text(function call_func, function abort_func, mixed* args ... )
{
  write("Type ** on a line by itself to end; ~q to quit.\n");
  write("1>");
  input_to( (:get_text:), 0, 2, call_func, abort_func, args );
}

void
get_text(string input, int count, function call_func, function abort_func,
	 mixed* args)
{
  switch(input) {
    case "**":
      evaluate(call_func, lines, args ... );
      destruct(this_object());
      break;
    case "~q":
      if(abort_func)
	evaluate(abort_func, lines, args ... );
      destruct(this_object());
      break;
    default:
      lines += ({ input });
      printf("%d>", count++);
      input_to( (:get_text:), 0, count, call_func, abort_func, args );
  }
}
