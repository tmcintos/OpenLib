// test.c:  (Tim) This is just a test command to make sure argv, flags, etc.
//          is getting the right things.
//
//          argv:  should be an array of all the words passed to the command
//          argc:  should be the size of 'argv'
//          flags: should be an array of all flags passed to the command
//          flag(str) tells you if the 1-char string 'str' flag is present
//
//          Longer flags like -Pln03r should be able to be picked out of
//          argv using member_array

#include <command.h>
#include <daemons.h>
#include <cmdline.h>
#define NL +"#end#\n"

int
_main(string args, int argc, string *argv, string *flags)
{
  write("args: "+ args NL);
  write("argv: "+ dump_variable(argv));
  write("argc: "+ argc NL);
  write("flags: "+ dump_variable(flags));
  message("test", "This is a test.\n", this_player());
  write("L_D"+LOGIN_D+"\n");
  write("V_D"+VIRTUAL_D+"\n");
  return 1;
}
