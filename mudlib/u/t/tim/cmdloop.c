/*  -*- LPC -*-  */
#include <mudlib.h>

inherit OBJECT;

string command = "";

int enterloop();
void loop(string ch);

void init()
{
  add_action((: enterloop :), "enterloop");
}

int
enterloop()
{
  get_char((: loop :), 1);
}

void
loop(string ch)
{
  string verb, args;

  switch(ch) {
  case "\r":
    if(sscanf(command, "%s %s", verb, args) != 2) {
      verb = command;
      args = 0;
    }
    printf("command: %s\nargs: %s\n", verb, args);
    command = "";
    break;
  case "":
    command = command[0..<2];
    write("\b\b ");
    break;
  case "q":
    return;
  default:
    command += ch;
    write(ch);
  }
  get_char((: loop :), 1);
}
