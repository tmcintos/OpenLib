/*  -*- LPC -*-  */
// valid.c:  included by master.c
/* security -- whole thing */
// separated out from master.c to reduce the complexity a bit.

int
valid_shadow(object ob)
{
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

int
valid_bind(object binder, object old_owner, object new_owner)
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
  return 1;
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

int
valid_asm(string file)
{
  return 1;
}
