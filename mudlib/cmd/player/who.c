/*  -*- LPC -*-  */
#include <command.h>
#include <login.h>

int
main(string arg)
{
  int screen_wid = 80;
  string line =
"---------------------------------------------------------------------------"
"----------------------------------";
  object user;
  int j;

  write("Players on " + mud_name() + " at " + ctime(time()) +
	" (*edit, +input)\n");
  write(line[0..screen_wid - 2] + "\n");

  foreach(user in users()) {
    if(user->short())
      write((in_edit(user) ? "*" : " ") + (in_input(user) ? "+" : " ") +
	    (string)user->short()+".\n");
    else
      write("(no descrip -- this should not happen)\n");
  }

  return 1;
}
