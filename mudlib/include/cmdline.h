/*  -*- LPC -*-  */
// cmdline.h:  Tim McIntosh (astasia@iastate.edu)
// functions for getting various things from the command line
//
// #include this and write _main() as declared below for your main() function.
//
// Inspired by VikingMUD and other things...
//
//          argv:  should be an array of all the words passed to the command
//          argc:  should be the size of 'argv'
//          flags: should be an array of all flags passed to the command
//          flag(str) tells you if the 1-char string 'str' flag is present
//
//          Longer flags like -Pln03r should be able to be picked out of
//          argv using member_array

int main(string arguments);
int flag(string *flags, string flag);
int _main(string args, int argc, string *argv, string *flags);

int
main(string arguments)
{
  int argc;                     //  size of argv
  string *argv = ({});          //  array of verb + args passed to this command
  string *flags = ({});         //  array of flags passed
  string args;                  //  Argments to command with flags taken out

  int i;
  string *atmp = ({});

  if(arguments) 
    argv = explode(arguments, " ");
  argv = ({ query_verb() }) + argv;

  argc = sizeof(argv);

  for(i=0; i < argc; i++) {               // get array of flags
    if(argv[i][0] == '-')
      flags += explode(argv[i], "");
    else if(i != 0)
      atmp += ({ argv[i] });              // get words that aren't flags
  }

// Remember argv[0] is the verb.
  args = implode(atmp, " ");
  if(!arguments) args = 0;

  return _main(args, argc, argv, flags);
}

//  funcion: flag         Arguments:  flags array and a flag ex: "l", "r", etc.
//                        Returns:  1 if flag was passed
//                                  0 if flag was not passed

int
flag(string *flags, string flag)
{
  if(member_array(flag, flags) != -1)
    return 1;

  return 0;
}
