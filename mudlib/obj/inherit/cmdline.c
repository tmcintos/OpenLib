/*  -*- LPC -*-  */
// cmdline.c:  facilitates getting various things from the command line
// written by: Tim McIntosh (astasia@iastate.edu)
//
// This file is part of the UltraLib distribution mudlib.
// It may be used and modified as long as this header remains intact.
// The UltraLib mudlib is available via FTP at knapp38.res.iastate.edu
//
// Inspired by VikingMUD and other things...
//

int _main(string arguments);
int flag(string flag);

// To be defined by object which inherits this
int main(string *argv, string *argv2);

private string *flags;               //  array of flags passed on command line

// Function:  _main()
// Arguments:  command line as taken from player
// Returns:  value returned by _main()
//
// This _main() function sits in front of the main() function to strip
// out flags and separate all the words on the command line

int
_main(string arguments)
{
  string *argv = ({});          //  array of verb + all args passed to command
  string *argv2 = ({});         // array of all non-flag arguments

  flags = ({});

  if(arguments) 
    argv = explode(arguments, " ");

  argv = ({ query_verb() }) + argv;

  for(int i=0; i < sizeof(argv); i++) {         // get array of flags
    if(argv[i][0] == '-') {
      if(argv[i][1] != '-')
	flags += explode(argv[i], "");          // normal flags
      else
	flags += ({ argv[i][2..] });            // word flags passed as --word
    } else if(i != 0) {
      argv2 += ({ argv[i] });                   // get words that aren't flags
    }
  }

  // Might as well catch this here

  if(flag("help")) {
    printf("Help for %s:\n\n", argv[0]);
    write(this_object()->help());
    write("\n");
    return 1;
  }

  return main(argv, argv2);
}

// Function:  flag()
// Arguments:  a flag ex: "l", "r", "help", etc.
// Returns:  1 if flag was passed
//           0 if flag was not passed

int
flag(string flag)
{
  return member_array(flag, flags) != -1;
}
