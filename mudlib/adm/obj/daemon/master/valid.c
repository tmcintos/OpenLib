/*  -*- LPC -*-  */
// valid.c:  included by master.c

// separated out from master.c to reduce the complexity a bit.

#include <uid.h>

int
valid_shadow(object ob)
{
#if 0
    if (getuid(ob) == ROOT_UID) {
        return 0;
    }
    if (ob->query_prevent_shadow(previous_object())) {
        return 0;
    }
#endif
	return 1;
}

int
valid_author(string name)
{
	return 1;
}

// valid_override: controls which simul_efuns may be overridden with
// efun:: prefix and which may not.  This function is only called at
// object compile-time.
//
// returns: 1 if override is allowed, 0 if not.

int
valid_override(string file, string name)
{
	if (file == SIMUL_EFUN) {
		return 1;
	}
	if ((name == "move_object") && (file != BASE))
		return 0;
//  may also wish to protect destruct, shutdown, snoop, and exec.
	return 1;
}

// valid_seteuid: determines whether an object ob can become euid str.
// returns: 1 if seteuid() may succeed, 0 if not.

int
valid_seteuid(object ob, string str)
{
    return 1;
}

// valid_domain: decides if a domain may be created
// returns: 1 if domain may be created, 0 if not.

int
valid_domain(string domain)
{
	return 1;
} 

// valid_socket: controls access to socket efunctions
// return: 1 if access allowed, 0 if not.

int
valid_socket(object eff_user, string fun, mixed *info)
{
	return 1;
}

// Write and Read privileges:
//
// valid_write: called with the file name, the object initiating the call,
//  and the function by which they called it. 
// return: 1 if access allowed, 0 if access not allowed.

int
valid_write(string file, mixed user, string func)
{
  boolean ret = 1;
  string *dir;
  object *stack, ob;

// I'll fix this crap later..Tim.
  return 1;

  dir = explode(file, "/");

  stack = call_stack(1);

#if 0
    tell_object(find_player("tim"),
		"valid_write:\n"
		"  file: " + file + "\n"
		"  func: " + func + "\n"
		+ dump_variable(stack));
#endif

// First test for members of the ROOT group...no need to go further otherwise
  foreach(ob in stack) {
    if(!GROUP_D->is_member(ROOT_GID, geteuid(ob)))
      ret = 0;
  }
  if(ret) return 1;          // if everyone was a member, we're ok

// Ok we're not root..so let's do the tests
  switch ("/" + dir[0]) {
  case TMP_DIR:              // anyone can write here
    return 1;

  case SECURE_DIR:           // only group root can write here, and we've
    return 0;                // already verified they're not root

  case DATA_DIR:             // group system and mudlib need to access this
    foreach(ob in stack) {
      if(!(GROUP_D->is_member(MUDLIB_GID, geteuid(ob)) ||
	   GROUP_D->is_member(SYSTEM_GID, geteuid(ob))))
	return 0;
    }
    return 1;

  case CMD_DIR:              // only group mudlib needs to access these
  case DOC_DIR:
  case INCLUDE_DIR:
  case OBJECT_DIR:
    foreach(ob in stack) {
      if(!GROUP_D->is_member(MUDLIB_GID, geteuid(ob)))
	return 0;
    }
    return 1;

  case DOMAIN_DIR:           // domain groups and group mudlib need access
    if(sizeof(dir) >= 2) {   // to the domain directories
      foreach(ob in stack) {
	if(!(GROUP_D->is_member(dir[1], geteuid(ob)) ||
	     GROUP_D->is_member(MUDLIB_GID, geteuid(ob))))
	  return 0;
      }
      return 1;
    }
    return 0;                // trying to access /d, and we're not root

  case USER_DIR:             // home directories; only the user needs access
    if(sizeof(dir) >= 3) {   // for now anyway
      foreach(ob in stack) {
	if(geteuid(ob) == dir[2])
	  return 1;
      }
    }

    foreach(ob in stack) {
      if(!GROUP_D->is_member(SYSTEM_GID, geteuid(ob)))
      	return 0;               // trying /u or /u/* and not root or system
    }
    return 1;                   // member of system

  default:
    return 0;                // root directory and whatever else
  }
}

// valid_read:  called exactly the same as valid_write()

int
valid_read(string file, mixed user, string func)
{
    return 1;
}

int
valid_save_binary(string file)
{
  return 1;
}

int
valid_compile_to_c(string file)
{
  return 1;
}
