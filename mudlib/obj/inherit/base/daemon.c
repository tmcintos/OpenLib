/*  -*- LPC -*-  */
// daemon.c
// inherited by daemons (go figure) and commands
// 

int no_clean;       // 1 if we don't want cleaned up

void
create()
{
  seteuid(getuid(this_object()));
  no_clean = 0;
}

void
remove()
{
  destruct(this_object());
}


// Only here for Intermud3 compatability... (remove later)

void
Destruct()
{
  remove();
}

// You can't shadow daemons

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

void
SetNoClean(int i)
{
  no_clean = i;
}
