/*  -*- LPC -*-  */
// daemon.c
// inherited by daemons (go figure) and commands
// 
#pragma save_binary

private int no_clean;       // 1 if we don't want cleaned up

void
create()
{
  seteuid(getuid(this_object()));
  no_clean = 0;
}

void
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

void
clean_up(int inh)
{
  if(no_clean) return 0;         // i.e. never call clean_up again
  remove();
}

nomask
void
SetNoClean(int i)
{
  no_clean = i;
}
