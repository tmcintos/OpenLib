/*  -*- LPC -*-  */
// command.c: inherited by all commands
// written by: Tim McIntosh (astasia@iastate.edu)
//
// This file is part of the UltraLib distribution mudlib.
// It may be used and modified as long as this header remains intact.
// The UltraLib mudlib is available via FTP at knapp38.res.iastate.edu
//
// Inspired by VikingMUD and other things...
//

int _main(string arguments);

// This is defined by the command
varargs int main(string args);

int
_main(string args)
{
  return main(args);
}
