/*  -*- LPC -*-  */
// queue.c:  Object which implements a queue
// written by Tim 2/8/96
//
// See queue.h for documentation

#include "queue.h"

private mixed* queue;                    // queue[0] is first out

void
create()
{
  queue = ({});
}

int
save(string file)
{
  return save_object(file);
}

int
restore(string file)
{
  return restore_object(file);
}

void
queue(mixed item)
{
  queue += ({ item });
}

mixed
pull()
{
  mixed ret = queue[0];

  queue = queue[1..];

  return ret;
}

boolean
queued(mixed item)
{
  return member_array(queue, item) != -1;
}

void
dequeue(mixed item)
{
  queue -= ({ item });
}

boolean
is_empty()
{
  return (!sizeof(queue));
}
