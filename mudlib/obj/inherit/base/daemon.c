/*  -*- LPC -*-  */
// daemon.c
// inherited by daemons (go figure) and commands
// 
#pragma save_binary
#include "daemon.h"

void
create()
{
  seteuid(getuid(this_object()));
}

int
remove()
{
// code to be done on destruct() here
}


// Only here for Intermud3 compatability... (remove later)

void
Destruct()
{
  destruct(this_object());
}

// You can't shadow daemons

nomask
int
query_prevent_shadow()
{
  return 1;
}

nomask
void
SetNoClean(int i)
{
}
