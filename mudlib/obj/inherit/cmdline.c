/*  -*- LPC -*-  */
// cmdline.c:  facilitates getting various things from the command line
// written by: Tim McIntosh (astasia@iastate.edu)
//
// This file is part of the UltraLib distribution mudlib.
// It may be used and modified as long as this header remains intact.
//
// See the files in /doc/USAGE/ for details reguarding the use, modification,
// and copying of this code.
//
// Inspired by VikingMUD and other things...
//
// 05/03/96  Tim : optimized (?) some by changing the way flags are handled.

int _main(string arguments);
int flag(string flag);

// To be defined by object which inherits this
int main(string *argv, string *argv2);

private string *flags;               //  array of flags passed on command line
private string charflags;            //  single character flags

// Function:  _main()
// Arguments:  command line as taken from player
// Returns:  value returned by _main()
//
// This _main() function sits in front of the main() function to strip
// out flags and separate all the words on the command line

int
_main(string arguments)
{
  string* argv;          //  array of verb + all args passed to command
  string* argv2;         // array of all non-flag arguments

  flags = ({});
  charflags = "";

  if( arguments )
  {
    argv =  explode(arguments, " ");
    argv2 = filter(argv, (: strsrch($1, '-') :));  // remove if begins with '-'
  }
  else
  {
    argv =  ({ });
    argv2 = ({ });
  }

  foreach(string word in filter(argv, (: !strsrch($1, '-') :)))
  {
    if( word[1] != '-' )
      charflags += word;                   // character flags
    else
      flags += ({ word[2..] });            // word flags passed as --word
  }

  // Might as well catch this here

  if( flag("help") )
  {
    printf("Help for %s:\n\n", argv[0]);
    write(this_object()->help());
    write("\n");
    return 1;
  }

  return main(({ query_verb() }) + argv, argv2);
}

// Function:  flag()
// Arguments:  a flag ex: "l", "r", "help", etc.
// Returns:  1 if flag was passed
//           0 if flag was not passed

int
flag(string flag)
{
  if( sizeof(flag) == 1 )
    return strsrch(charflags, flag) != -1;
  else
    return member_array(flag, flags) != -1;
}
