/*  -*- LPC -*-  */
// security_d.c:  Mudlib security implementation
//   Created by Tim 04.13.96
//
//   Based on security concept by Reimer Behrends
//   (behrends@buran.fb10.tu-berlin.de) described in document "LIMA MUDLIB
//   SECURITY" on Lima Bean homepage (http://lima.imaginary.com/security.html).

// todo: 
//       valid_* funcs called from master (read, write, etc)
//       work on privs_file() master apply
//       efun files--clean them up, write more code
//       verify file protection, clean up check_privilege
//       verify that everything works correctly--robustness, make sure you
//         can't break it by passing weird values. (also warning msgs etc.)
//       insert unguarded() calls into the proper objects.
//       figure out file protections for user data & how to set up.
//       move objects to the right directories (move include to /adm/include)
//       figure out how to save a player body when a file protection is not 0
//         probably involves reworking the connection/body scheme so the
//         body can have higher privs than the connection.
// files modified lately:
// none

#include <dirs.h>
#include <origin.h>
#include <security.h>

#define DOMAIN_MEMBER 1
#define DOMAIN_ADMIN  2

/*
 * Protos
 */
nomask int admin_domain(string domain, string name);
nomask int member_domain(string domain, string name);
nomask string privs_file(string filename);

/*
 * Globals
 */
private static object unguarded_ob;
private static mixed  unguarded_priv;
/*
 * mapping of absolute paths (beginning with '/') to protections
 */
private mapping write_protections;
private mapping read_protections;
/*
 * mapping of mappings:
 * domain names -> ( member names )-> ( 1 == admin || 2 == member )
 */
private mapping domains;

/*
 * privs[x] -> ({ list })
 * where 'x' is some privilege and 'list' is an array of privileges which
 * priv x is opened to. (any priv can be opened to any other)
 *
 * 4 types of privs:
 *   1. integer 0 or 1; 0 == lowest, 1 == highest
 *   2. string of alphabetic lower case characters.
 *      There is a 1 to 1 correspondence between these privs & wizards.
 *   3. same as (2) but with 1st letter capitalized.
 *      There is a 1 to 1 correspondence between these privs & domains.
 *   4. The @ character followed by a string of lower case alphabetic chars.
 *      These can only be created by admins.
 */
private mapping privs;

/*
 *   An object created from file 'f' may only have a privilege as high as
 * the priv (or protection) 'f' is protected by.
 *   To access something protected by priv 'p' you have to have a priv
 * higher than 'p', or 'p' must be opened to the priv that you have.
 *   This way an object created from a directory protected by "tim:" does
 * not have access to the directory it was created from.
 *   The special case of course is priv '1' since it will always test higher
 * than any other priv including itself.  A similar case occurs with '0',
 * as it will always test lower than any other priv including itself.
 *   To make it possible for someone with a control priv to create other
 * privs, the control priv must be opened to itself.
 */

nomask void
create()
{
  domains = ([ ADMIN_DOMAIN : ([ ROOT_PRIV : DOMAIN_ADMIN ]) ]);
  privs = ([
	    1               : ({ ADMIN_DOMAIN }),
	    ROOT_PRIV       : ({ }),
	    ROOT_PRIV ":"   : ({ }),
	    ADMIN_DOMAIN    : ({ ROOT_PRIV }),
	    ADMIN_DOMAIN ":": ({ }),
	    MUDLIB_PRIV     : ({ ADMIN_DOMAIN }),
	    0               : ({ }),
	    ]);
  write_protections = ([
			"/"        : save_variable(1),
			TMP_DIR    : save_variable(0),
			PUB_DIR    : save_variable(0),
			SECURE_DIR : save_variable(1),
			CMD_DIR    : save_variable(MUDLIB_PRIV),
			DOMAIN_DIR : save_variable(MUDLIB_PRIV),
			DATA_DIR   : save_variable(MUDLIB_PRIV),
			SECURE_DATA_DIR   : save_variable(0), // temporary!
			DOC_DIR    : save_variable(MUDLIB_PRIV),
			FTP_DIR    : save_variable(MUDLIB_PRIV),
			INCLUDE_DIR: save_variable(MUDLIB_PRIV),
			LOG_DIR    : save_variable(MUDLIB_PRIV),
//			OBJECT_DIR : save_variable(MUDLIB_PRIV),
			OBJECT_DIR : save_variable(1),      // temporary!
//			USER_DIR   : save_variable(MUDLIB_PRIV),
			USER_DIR   : save_variable(0),      // temporary!
		      ]);
  read_protections = ([]);

  unguarded((: restore_object( SAVE_FILE, 1 ) :), 1);
}

nomask void
save_me()
{
  unguarded((: save_object( SAVE_FILE ) :), 1);
}

nomask int
valid_priv(mixed p)
{
  if( intp(p) ) return ( p == 1 || p == 0 );

  if( p[0] == '@' ) {
    if( strsrch(p, ':') > -1 )  // no colon in these
      return 0;
  } else {
    string* tmp = explode(p, ":");

    if( sizeof(tmp) > 2 )
      return 0;        // colon appears < 1 time.
    p = implode(tmp, "");

    if( p[0] < 'A' || p[0] > 'z' || (p[0] > 'Z' && p[0] < 'a') )
      return 0;
  }

  p = p[1..];   // 1st letter can be upper or lower case or @

  foreach(int ch in p)
    if( ch < 'a' || ch > 'z' ) return 0;  // rest must be lower case

  return 1;
}

nomask mixed*
get_privs()
{
  return keys(privs);
}

nomask int
priv_exists(mixed priv)
{
  return (privs[priv] ? 1 : 0);
}

//
// return control priv for priv 'p'
// NOTE: control priv need not actually exist, it just returns what _would_
//       be the control priv for 'p' (of course if 'p' exists, the control
//       priv must exist also).
//
nomask mixed
get_control_priv(mixed p)
{
  int idx;

  if( intp(p) ) return p;
  idx = strsrch(p, ':');

  if( idx < 0 ) return p;  // this _is_ the control priv
  return p[0..idx - 1];
}

// is p1 a subpriv of p2?
nomask int
is_subpriv(mixed p1, mixed p2)
{
  if( intp(p1) ) return 0;  // 1 and 0 are never true subprivs

  if( p1 != p2 &&
      get_control_priv(p1) == get_control_priv(p2) &&
      !strsrch(p1, p2) )                              // p2 is a prefix of p1
    return 1;  

  return 0;
}

//
// return subprivileges of priv 'p'
// NOTE: 'p' need not exist, just returns theoretical subprivs of 'p'
//
nomask string*
get_subprivs(mixed p)
{
  if( intp(p) )
    return (p ? keys(privs) : ({}));

  return filter(keys(privs), (: is_subpriv($1, $(p)) :));
}

//
// is p1 opened up to p2?
//
// PRE: privs passed to this exist
nomask int
is_opened(mixed p1, mixed p2)
{
  /*
   * p1 is opened up to p2 directly
   */
  if( member_array(p2, privs[p1]) != -1 ) return 1;
  /*
   * If p1 == p2 and it is a wizard's control priv. (always opened to itself)
   * This makes it so check_priv("tim") will succeed for a wizard with priv
   * "tim" allowing him to create data privileges.
   */
  if( p1 == p2 && p1[0] != '@' && strsrch(p1, ':') < 0 && !domains[p1] )
    return 1;
  /*
   * If p1 is a domain control priv and p2 is a domain admin
   */
  if( domains[p1] && admin_domain(p1, p2) ) return 1;
  /*
   * if p2 is an admin or member of any domains which p1 is opened to...
   */
  foreach(string domain in filter(privs[p1], (: stringp($1) :)))
  {
    if( domain[<1] != ':' ) {
      if( domains[domain] && admin_domain(domain, p2) )
	return 1;
    } else {
      domain = domain[0..<2];
      if( domains[domain] && member_domain(domain, p2) )
	return 1;
    }
  }
  return 0;
}

//
// is p1 greater than p2?
//
nomask int
is_greater(mixed p1, mixed p2)
{
  /*
   * if p1 is 1 then it must be greater, if it is 0 then it must be less
   * reverse is true for p2
   */
  if( p1 == 1 ) return 1;
  if( p2 == 0 ) return 1;
  if( p1 == 0 ) return 0;
  if( p2 == 1 ) return 0;
//  if( intp(p1) ) return p1;
//  if( intp(p2) ) return (!p2);   
  if( p1 != p2 &&
      get_control_priv(p1) == get_control_priv(p2) &&
      !strsrch(p2, p1) )                              // p1 is a prefix of p2
    return 1;  

  /*
   * this is probably the fastest way to check this
   */
  switch( p2[0] ) {
  case 'A'..'Z': /* Domain Priv */
    {
      string domain = p2;
      int idx = strsrch(domain, ':');
      /*
       * Domain admins hold the control priv, so they are greater than any
       * other domain priv, but not the control priv itself.
       * Domain members hold the domain data priv, so they are greater than
       * any other domain data priv, but not the first domain data priv itself.
       */
      if( idx > -1 ) {
	if( domain[<1] == ':' )
	  return admin_domain(domain[0..<2], p1);
	else
	  return member_domain(domain[0..idx-1], p1);
      }
    }
  }
  return 0;
}

/*
 * this_object() is always popped off the top of the stack first (not checked)
 * 'skip' tells how many other objects to pop off the stack before checking.
 */
nomask int
check_privilege(mixed priv, int skip)
{
  object* stack = call_stack(1);
  string* funcs = call_stack(2);

//  message("none", dump_variable(stack), find_player("tim"));

  for(int i = skip + 1; i < sizeof(stack); i++) {
    mixed ob_priv = query_privs(stack[i]);
    int stop;

    if( !stack[i] )
      error("Object in call stack was destructed.\n");

    if( stack[i] == master() || stack[i] == simul_efun() )
      continue;

    if( stack[i] == this_object() ) {
      if( funcs[i] == "eval_unguarded" ) {
	ob_priv = unguarded_priv;
	stop = 1;
      } else
	continue;
    }

//    if(find_player("tim"))
//    message("none", sprintf("%O (%s) ==> %O\n", stack[i], funcs[i], ob_priv),
//	    find_player("tim"));

    if( !is_greater(ob_priv, priv) && !is_opened(priv, ob_priv) ) {
//      message("none", "is_greater() failed, returning 0\n", find_player("tim"));
      return 0;
    }
    if( stop ) {
//      message("none", "--would stop here--\n", find_player("tim"));
      break;
    }
  }
//      message("none", "checked ok, returning 1\n", find_player("tim"));
  return 1;
}

nomask mixed
eval_unguarded(function code, mixed priv)
{
  object tmp_ob = unguarded_ob;
  mixed ret, tmp_priv = unguarded_priv;
  mixed ob_priv = query_privs(previous_object(1));
  string err;

  if( previous_object() != simul_efun() )
    error("Illegal call to eval_unguarded()");

  /*
   * This has to be done this way since 1 always tests higher.
   * (even against itself)
   * Similar special case for 0.
   */
  if( priv != ob_priv &&
      is_greater(priv, ob_priv) &&
      !is_greater(ob_priv, priv) )
    error("Cannot set privilege level higher than object calling unguarded");

  unguarded_ob = previous_object(1);
  unguarded_priv = priv;
  err = catch(ret = evaluate(code));
  unguarded_ob = tmp_ob;
  unguarded_priv = tmp_priv;

  if( err ) error(err);
  return ret;
}

//
// create a new privilege 'name'
//
nomask string
create_priv(string name)
{
  mixed cp = get_control_priv(name = lower_case(name));

  if( !name || !valid_priv(name) )
    return (string) notify_fail("That is not a valid privilege name.\n");

  if( name[0] != '@' ) {
    if( name == cp )
      return (string) notify_fail("Only non-data privileges beginning with "
				  "'@' can be created manually.\n");
    else if( !check_previous_priv(cp) )
      return (string) notify_fail("Insufficient privileges.\n");
  } else {
    if( name == cp ) {
      if( !check_previous_priv(1) )
	return (string) notify_fail("Insufficient privileges.\n");
    } else {
      if( !check_previous_priv(cp) )
	return (string) notify_fail("Insufficient privileges.\n");
    }
  }

  if( !privs[cp] && name != cp )
    return (string) notify_fail("That is not a valid data privilege.\n");

  if( privs[name] )
    return (string) notify_fail("Privilege already exists.\n");

  privs[name] = ({ });
  save_me();
  return name;
}

private void
remove_priv(string name)
{
  string* privlist;

  if( name == get_control_priv(name) )
    privlist = get_subprivs(name) + ({ name });
  else
    privlist = ({ name });

  // delete associated privileges
  foreach(string p in privlist)
    map_delete(privs, p);

  // delete associated privileges from "opened to" lists of other privs
  foreach(mixed p in keys(privs))
    privs[p] -= privlist;

  // delete file protections which use the deleted privs since they are useless
  foreach(string file, string p in write_protections)
    if( member_array(restore_variable(p), privlist) > -1 )
      map_delete(write_protections, file);
  foreach(string file, string p in read_protections)
    if( member_array(restore_variable(p), privlist) > -1 )
      map_delete(read_protections, file);
}

// delete privilege 'name'
nomask int
delete_priv(mixed name)
{
  if( !stringp(name) )
    return notify_fail("Can only delete string type privileges.\n");

  if( name[0] != '@' && strsrch(name, ':') < 0 || name[<1] == ':' )
    return notify_fail("Cannot delete wizard or domain privileges "
		       "manually.\n");

  if( !privs[name] )
    return notify_fail("No such privilege: " + name + "\n");

  if( !check_previous_priv(get_control_priv(name)) )
    return notify_fail("Insufficient privileges.\n");

  remove_priv(name);
  save_me();
  return 1;
}

//
// (user command support) return the "opened to" list for a priv 'p'
//
nomask mixed*
get_opened_list(mixed p)
{
  return copy(privs[p]);
}

//
// (user command support) open priv1 to priv2
//
nomask int
open_priv(string priv1, string priv2)
{
  if( !privs[priv1] )
    return notify_fail("No such privilege: " + priv1 + "\n");

  if( !privs[priv2] )
    return notify_fail("No such privilege: " + priv2 + "\n");

  if( !check_previous_priv(priv1) )
    return notify_fail("Insufficient privileges.\n");

  if( is_opened(priv1, priv2) )
    return notify_fail(sprintf("'%s' is already opened to '%s'. (directly or "
			       "indirectly)\n", priv1, priv2));

  privs[priv1] += ({ priv2 });

  save_me();
  return 1;
}

//
// (user command support) close priv1 to priv2
//
nomask int
close_priv(string priv1, string priv2)
{
  if( !privs[priv1] )
    return notify_fail("No such privilege: " + priv1 + "\n");

  if( !privs[priv2] )
    return notify_fail("No such privilege: " + priv2 + "\n");

  if( !check_previous_priv(priv1) )
    return notify_fail("Insufficient privileges.\n");

  if( !is_opened(priv1, priv2) )
    return notify_fail(sprintf("'%s' is not opened to '%s'. (directly or "
			       "indirectly)\n", priv1, priv2));

  privs[priv1] -= ({ priv2 });

  save_me();
  return 1;
}

nomask string*
get_wizards()
{
  return filter(filter(filter(keys(privs), (: stringp($1) :)),
		       (: strsrch($1, ':') < 0 && $1[0] != '@' :)),
		(: $1 == lower_case($1) :));
}

nomask int
wizard_exists(string name)
{
  return (member_array(lower_case(name), get_wizards()) != -1);
}

//
// (user command support)
//
nomask string
create_wizard(string name)
{
  object wiz;

  name = lower_case(name);

  if( !user_exists(name) )
    return (string) notify_fail("A player must exist to become a wizard!\n");

  if( !valid_priv(name) || name[0] == '@' || strsrch(name, ':') > -1 )
    return (string) notify_fail("That is not a valid wizard name.\n");

  if( !check_previous_priv(1) )
    return (string) notify_fail("Insufficient privileges.\n");

  if( wizard_exists(name) )
    return (string) notify_fail("Wizard already exists.\n");

  privs[name] = ({ });
  privs[name+":"] = ({ });

  if( wiz = find_player(name) )
    unguarded((: set_privs($(wiz), $(name)) :), 1);

  save_me();
  return name;
}

//
// (user command support)
//
nomask int
delete_wizard(string name)
{
  object wiz;

  if( !check_previous_priv(1) )
    return notify_fail("Insufficient privileges.\n");

  if( !wizard_exists(name) )
    return notify_fail("No such wizard.\n");

  // delete from member lists of domains
  foreach(string domain in keys(domains)) {
    if( domains[domain][name] )
      map_delete(domains[domain], name);
  }

  remove_priv(name);

  if( wiz = find_player(name) )
    unguarded((: set_privs($(wiz), 0) :), 1);

  save_me();
  return 1;
}

//
// (user command support)
//
nomask string*
get_domains()
{
  return keys(domains);
}

nomask int
domain_exists(string name)
{
  return (domains[name] ? 1 : 0);
}

//
// (user command support)
//
nomask string
create_domain(string name)
{
  name = capitalize(lower_case(name));

  if( !valid_priv(name) || name[0] == '@' || strsrch(name, ':') > -1 )
    return (string) notify_fail("That is not a valid domain name.\n");

  if( !check_previous_priv(1) )
    return (string) notify_fail("Insufficient privileges.\n");

  if( domains[name] )
    return (string) notify_fail("Domain already exists.\n");

  domains[name] = ([ ]);
  privs[name] = ({ });
  privs[name+":"] = ({ });

  save_me();
  return name;
}

//
// (user command support)
//
nomask int
delete_domain(string name)
{
  if( !check_previous_priv(1) )
    return notify_fail("Insufficient privileges.\n");

  if( !domains[name] )
    return notify_fail("No such domain.\n");

  // delete from domain list
  map_delete(domains, name);

  remove_priv(name);

  save_me();
  return 1;
}

nomask int
admin_domain(string domain, string name)
{
  if( !domains[domain] ) return 0;
  return domains[domain][name] == DOMAIN_ADMIN;
}

nomask int
member_domain(string domain, string name)
{
  if( !domains[domain] ) return 0;
  return (domains[domain][name] ? 1 : 0);
}

//
// (user command support)
//
nomask int
add_domain_member(string domain, string member)
{
  object mem;

  member = lower_case(member);

  if( !domains[domain] )
    return notify_fail("No such domain.\n");

  if( !wizard_exists(member) )
    return notify_fail("No such wizard.\n");

  if( domains[domain][member] )
    return notify_fail(capitalize(member) +" is already a member!\n");

  if( !check_previous_priv(domain) )
    return notify_fail("Insufficient privileges.\n");

  domains[domain][member] = DOMAIN_MEMBER;

  if( mem = find_player(member) )
  {
    if( domain == ADMIN_DOMAIN )
      unguarded((: set_privs($(mem), 1) :), 1);
    message("system", "You are now a member of domain "+ domain +".\n", mem);
  }

  save_me();
  return 1;
}

//
// (user command support)
//
nomask int
remove_domain_member(string domain, string member)
{
  object mem;

  member = lower_case(member);

  if( !domains[domain] )
    return notify_fail("No such domain.\n");

  if( !domains[domain][member] )
    return notify_fail(sprintf("%s is not a member of domain %s.\n",
			       capitalize(member), domain));

  if( !check_previous_priv(domain) )
    return notify_fail("Insufficient privileges.\n");

  if( admin_domain(domain, member) )
    privs[domain] -= ({ member });

  map_delete(domains[domain], member);

  if( mem = find_player(member) )
  {
    if( domain == ADMIN_DOMAIN )
      unguarded((: set_privs($(mem), $(member)) :), 1);
    message("system",
	    "You are no longer a member of domain "+ domain +".\n", mem);
  }

  save_me();
  return 1;
}

//
// (user command support)
//
nomask int
change_domain_status(string domain, string member, int admin)
{
  object mem;

  if( !domains[domain] )
    return notify_fail("No such domain.\n");

  if( !domains[domain][member] )
    return notify_fail(sprintf("%s is not a member of domain %s.\n",
			       capitalize(member), domain));

  if( !check_previous_priv(domain) )
    return notify_fail("Insufficient privileges.\n");

  if( admin )
  {
    if( domains[domain][member] == DOMAIN_ADMIN )
      return notify_fail(capitalize(member) + " is already an administrator "
			 "of that domain!\n");

    privs[domain] += ({ member });
  }
  else
  {
    if( domains[domain][member] == DOMAIN_MEMBER )
      return notify_fail(capitalize(member) + " is already a member "
			 "of that domain!\n");

    privs[domain] -= ({ member });
  }

  domains[domain][member] = (admin ? DOMAIN_ADMIN : DOMAIN_MEMBER);

  if( mem = find_player(member) )
    message("system", sprintf("You are now %s of domain %s.\n",
			      (admin ? "an administrator" : "a member"),
			      domain),
	    mem);

  save_me();
  return 1;
}

//
// (user command support)
//
nomask string*
get_domain_members(string dom)
{
  if( !domains[dom] )
    return ({ });
  return keys(domains[dom]);
}

//
// (user command support)
//
nomask string*
get_domain_memberships(string wiz)
{
  string* ret = ({ });

  foreach(string domain in keys(domains)) {
    if( domains[domain][wiz] )
      ret += ({ domain });
  }

  return ret;
}

//
// (user command support)
//
nomask mapping
get_protections(int read)
{
  if( read )
    return copy(read_protections);
  else
    return copy(write_protections);
}

// return the file protection for a file specified by an absolute path.
// if second argument is nonzero then read protections will be returned,
// otherwise write protections are returned.

nomask mixed
get_file_protection(string fname, int read)
{
  mapping protections = (read ? read_protections : write_protections);
  string prot;

  if( fname[0] != '/' )
    fname = "/" + fname;

  prot = protections[fname];

  while( !prot ) {
    int idx = strsrch(fname, '/', -1);

    // This makes sure that this loop will terminate
    // On read we default to 0, write to 1
    if( !idx )
      return (read ? 0 : 1);

    fname = fname[0..idx-1];
    prot = protections[fname];
  }

  return restore_variable(prot);
}

//
// (user command support)
//
// if second argument is nonzero then read protections will be affected,
// otherwise write protections are affected.

nomask int
protect_file(string fname, int read, mixed priv)
{
  mapping protections = (read ? read_protections : write_protections);

  // must have write permission to the file
  if( !check_previous_priv(get_file_protection(fname, 0)) )
    return notify_fail("Insufficient privileges.\n");

  if( !privs[priv] )
    return notify_fail("Attempted to apply invalid protection to a file.\n");

  // must have the priv we are trying to apply, since it will allow us to
  // create objects with that priv.
  if( !check_previous_priv(priv) )
    return notify_fail("Insufficient privileges.\n");

  protections[fname] = save_variable(priv);
  save_me();
  return 1;
}

//
// (user command support)
//
// if second argument is nonzero then read protections will be affected,
// otherwise write protections are affected.

nomask int
unprotect_file(string fname, int read)
{
  mapping protections = (read ? read_protections : write_protections);

  // must have write permission to the file
  if( !check_previous_priv(get_file_protection(fname, 0)) )
    return notify_fail("Insufficient privileges.\n");

  map_delete(protections, fname);
  save_me();
  return 1;
}

// called by the master object at object creation

nomask string
privs_file(string filename)
{
  mixed priv;

  // Security Daemon should always be 1
  if( file_name(this_object()) == "/"+filename )
    return save_variable(1);

  filename = "/" + explode(filename, "#")[0];
  priv = get_file_protection(filename, 0);

  return save_variable(priv);
}
