/*  -*- LPC -*-  */
// Simulated Efuns file

#include <dirs.h>
#include "simul_efun/efuns.h" // This file has all the efuns includes in it.

void
create()
{
  efun::set_privs(this_object(), save_variable(1));
}

varargs int
getoid(object ob)
{
  int id;

  if (!ob) {
    ob = previous_object();
  }
  sscanf(file_name(ob), "%*s#%d", id);
  return id;
}

// Get the owner of a file.  Used by log_error() in master.c.

string
file_owner(string file)
{
  string name, rest, dir;

  if (sscanf(file, USER_DIR "/%s/%s/%s", dir, name, rest) == 3) {
    return name;
  }
  return 0;
}

// domain_file should return the domain associated with a given file.

string
domain_file(string file)
{
  return "users";
}

// creator_file should return the name of the creator of a specific file.
// for setting uid

string
creator_file(string file)
{
  return "this should be gone";
}

// author_file should return the name of the author of a specific file.

string
author_file(string file)
{
  return "fix author_file";
}

object
simul_efun()
{
  return this_object();
}
