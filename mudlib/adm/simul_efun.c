/*  -*- LPC -*-  */
// Simulated Efuns file

#include <dirs.h>
#include <uid.h>
#include "efuns/efuns.h"   // This file has all the efuns includes in it...

void
create()
{
  seteuid(getuid(this_object()));
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

    if (file[0] != '/') {
        file = "/" + file;
    }
    if (sscanf(file, "/u/%s/%s/%s", dir, name, rest) == 3) {
        return name;
    }
    return 0;
}

// domain_file should return the domain associated with a given file.

string
domain_file(string file)
{
	return ROOT_UID;
}

// creator_file should return the name of the creator of a specific file.

string
creator_file(string file)
{
  string *dir;

  dir = explode(file, "/");

  switch("/"+dir[0]) {
  case SECURE_DIR:
    return ROOT_UID;
  case USER_DIR:
    if(sizeof(dir) >= 3)
      return dir[2];      // uid of user
    else
      return SYSTEM_UID;
  case DOMAIN_DIR:
    if(sizeof(dir) >= 2)
      return dir[1];      // name of domain
    else
      return SYSTEM_UID;
  case TMP_DIR:
    return NOBODY_UID;    // can't do anything really...
  default:
    return SYSTEM_UID;    // all nonsecure mudlib items use this
  }
}

// author_file should return the name of the author of a specific file.

string
author_file(string file)
{
	return ROOT_UID;
}
