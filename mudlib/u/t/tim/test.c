#include <mudlib.h>
inherit OBJECT;
int
get() {return 0;}

void
create(){
enable_commands();
}

void
write_cmds()
{
  write(dump_variable(commands()));
}
