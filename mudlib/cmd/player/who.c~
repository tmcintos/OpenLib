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

  write("Players on " + mud_name() + " at " + ctime(time()) + "\n");
  write(line[0..screen_wid - 1] + "\n");
  write(sprintf("%-50s idle\n", "name (*edit, +input)"));
  write(sprintf("%-50' 's ----\n", "--------------------"));

  foreach(user in users()) {
    if(user->short())
      write(sprintf("%-50s %4d\n",
		    (in_edit(user) ? "*" : " ") +
		    (in_input(user) ? "+" : " ") +
		    (string)user->short()+".",
		    query_idle(user) / 60
		    ));
    else
      write("(no descrip)\n");
  }

  return 1;
}
