#include <command.h>

int
main(string ob)
{
  if (!ob)
    return notify_fail("Usage: stat <object>\n");
  object the_obj = find_object(ob); /* should be more sophisticated, but it's
				       a pretty good start... */
  if (!the_obj)
    return notify_fail("Could not find object " + ob + "\n");
  mixed ret = the_obj->show_stats();
  if (arrayp(ret))
    this_player()->more(ret);
  if (stringp(ret))
  {
    write(ret);
    return 1;
  }
  if (!ret) /* does this mud have function_exists()? */
    return notify_fail("Object " + ob + " does not have a show_stats() fn\n");
  return 1;
}
