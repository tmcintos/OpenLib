/*
 * Banish daemon for the Ultralib mudlib. Written by Talen (Stuart Lamble -
 * lamble@yoyo.cc.monash.edu.au) January-February '96.
 *
 * This code is subject to the Ultralib copyright - specifically,
 * you may modify and use this code freely, but may not remove this copyright
 * from this file.
 *
 * Note that, since I (Talen) also wiz on Shattered Worlds, there may be
 * similarities between the code in this file, and the banish system on
 * SW. Any such similarities are coincidental, not intentional.
 */
/* Warning: this code is _completely_ untested... it may or may not work */
/*
 * Banish daemon: maintains a list of character names that have been
 * banished from the game, and (optionally) a list of times when they
 * can return. This time is of the form returned by time() - it is up
 * to any relevant objects to convert game time to real time if so
 * desired.
 */

/* should this be mixed *? */
string *names;
int    *times;
#define SAVE_NAME file_name(this_object())

void create()
{
names = ({ });
  times = ({ });
  restore_object(SAVE_NAME, 1);
  call_out("clean_out", 60);
}


save_me() { save_object(SAVE_NAME); }

void reset()
{
  save_me();
}

int clean_up(int inherited)
{
  return 0; /* this object should *always* stick around */
}

/* ultimately, will clean out the arrays of names that shouldn't be there. */
static clean_out()
{
  int loop = sizeof(names) - 1;
  int pos = member_array(names, 0);

  if (pos == -1) return; /* no zero elements! */

  for (; loop > pos; loop--)
  {
    if (names[loop])
    {
      names[pos] = names[loop];
      times[pos] = times[loop];
      names[loop] = 0;
      pos = member_array(names, 0);
    }
  }
  names = names[0..(pos - 1)]; /* compact it down to min size */
  times = times[0..(pos - 1)];
}

int banish(string name, int return_time)
{
/* put in a check for names that should never be banished here, if apt. */
  int pos;

  pos = member_array(names, name);
  if (pos == -1) /* not already there */
  {
    pos = member_array(names, 0); /* check for empty slots */
    if (pos == -1) /* there are no empties - create some. */
    {
      pos = sizeof(names);
      names += allocate(5);
      times += allocate(5);
    }
  }
  names[pos] = name;
  times[pos] = return_time;
  return 1;
}

int unban(string name)
{
  int pos = member_array(names, name);
  if (pos == -1) return 0; /* not there */
  names[pos] = 0;
  return 1;
}

int query_banned(string name)
{
  int pos = member_array(names, name);
  if (pos == -1) return 0; /* not there, not banished */
  if (!times[pos]) return -1; /* permanent banishment */
  if (times[pos] < time())
  {
    names[pos] = 0; /* unbanish them - their time is up. */
    return 0;
  }
  return times[pos]; /* still banished for however long. */
}
