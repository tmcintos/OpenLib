/*  -*- LPC -*-  */
// security_d.c:  Mudlib security implementation
//   Created by Tim 04.13.96
//
//   Based on security concept by Reimer Behrends
//   (behrends@buran.fb10.tu-berlin.de) described in document "LIMA MUDLIB
//   SECURITY" on Lima Bean homepage (http://lima.imaginary.com/security.html).

// todo: file protection, clean up check_privilege
//       valid_* funcs in master
//       take out uid stuff from lib
//       try out privs driver
//       fix query_privs efun and make set_privs efun
//       make privs_file() master apply
//       make commands related to setting up privs
//       remove unguarded define, fix existing code

#include <origin.h>

#undef unguarded

// This is for bootstrapping only really...provides a means for the first
// admin user.
#define ROOT_USER "tim"

#define SAVE_FILE "/adm/data/security"

#define DOMAIN_MEMBER 1
#define DOMAIN_ADMIN  2

/*
 * Protos
 */
nomask int admin_domain(string domain, string name);
nomask int member_domain(string domain, string name);

/*
 * Globals
 */
private object unguarded_ob;
private mixed  unguarded_priv;

/*
 * mapping of mappings:
 * domain names -> ( member names -> 1 == admin || 2 == member )
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

nomask void
create()
{
  domains = ([ "Admin" : ([ ROOT_USER : 1 ]) ]);
  privs = ([
	    1             : ({ "Admin" }),
	    ROOT_USER     : ({ }),
	    ROOT_USER ":" : ({ }),
	    "Admin"       : ({ ROOT_USER }),
	    "Admin:"      : ({ }),
	    ]);
  restore_object( SAVE_FILE, 1 );    // unguarded
}

nomask void
save_me()
{
  save_object( SAVE_FILE );      // unguarded
}

nomask int
priv_exists(string priv)
{
  return (privs[priv] ? 1 : 0);
}

// is p1 a subpriv of p2?
nomask int
is_subpriv(mixed p1, mixed p2)
{
  if( intp(p1) ) return (!p1);   // 1 is never a subpriv; 0 always is
  if( !strsrch(p1, p2) ) return 1;
  return 0;
}

// return subprivileges of priv 'p'
nomask string*
get_subprivs(mixed p)
{
  switch(p) {
  case 0:
    return 0;
  case 1:
    return keys(privs);
  default:
    return filter(keys(privs), (: is_subpriv($1, $(p)) :));
  }
}

// is p1 opened up to p2?
nomask int
is_opened(mixed p1, mixed p2)
{
  if( !privs[p1] ) return 0;

  if( member_array(p2, privs[p1]) != -1 ) return 1;   // p1 is opened up to p2
  return 0;
}

// return control priv for priv 'p'
nomask mixed
get_control_priv(mixed p)
{
  int idx;

  if( intp(p) ) return p;
  idx = strsrch(p, ':');
  if( idx < 0 ) return p;  // this _is_ the control priv
  return p[0..idx - 1];
}

// is p1 greater than (or equal to) p2?
// this also checks for domain membership and p2 being opened to p1
nomask int
is_greater(mixed p1, mixed p2)
{
  if( p1 == p2 ) return 1;
  /*
   * First the exceptions...
   *
   * p2 is opened up to p1
   */
  if( member_array(p1, privs[p2]) != -1 ) return 1;
  /*
   * if p1 is a member of any domains to which this p2 has been opened to...
   */
  foreach(string domain in filter(privs[p2], (: domains[lower_case($1)] :)))
    if( member_domain(domain, p1) ) return 1;
  /*
   * Now the rest...
   *
   * if p1 is 1 then it must be greater, if it is 0 then it must be less
   * reverse is true for p2
   */
  if( intp(p1) ) return p1;
  if( intp(p2) ) return (!p2);   
  /*
   * this is probably the fastest way to check this
   */
  switch( p2[0] ) {
  case 'A'..'Z': /* Domain Priv */
    {
      string domain = lower_case(p2);
      /*
       * if p2 is the domain control priv, must be an admin
       */
      if( domains[domain] && admin_domain(domain, p1) )
	return 1;
      /*
       * if p2 is a domain data priv, must be a member
       */
      else if( member_domain(get_control_priv(domain), p1) )
	return 1;
    }
  }
  if( !strsrch(p2, p1) ) return 1;                  // p1 is a prefix of p2
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

  for(int i = skip + 1; i < sizeof(stack); i++) {
    mixed ob_priv = query_privs(stack[i]);
    int stop;

    if( !stack[i] ) {
      error("Object in call stack was destructed.\n");
    }
    if( stack[i] == this_object() && funcs[i] == "eval_unguarded" ) {
      ob_priv = unguarded_priv;
      stop = 1;
    }
    message("none", sprintf("%O (%s) ==> %O\n", stack[i], funcs[i], ob_priv),
	    find_player("tim"));

    if( !is_greater(ob_priv, priv) ) return 0;
    if( stop ) {
      message("none", "--would stop here--\n", find_player("tim"));
      break;
    }
  }
  return 1;
}

nomask mixed
eval_unguarded(mixed priv, function code)
{
  object tmp_ob = unguarded_ob;
  mixed ret, tmp_priv = unguarded_priv;
  mixed ob_priv = query_privs(previous_object(1));
  string err;

  if( previous_object() != simul_efun() )
    error("Illegal call to eval_unguarded()");
  if( priv != ob_priv && is_greater(priv, ob_priv) )
    error("Cannot set privilege level higher than object calling unguarded");

  unguarded_ob = previous_object(1);
  unguarded_priv = priv;
  err = catch(ret = evaluate(code));
  unguarded_ob = tmp_ob;
  unguarded_priv = tmp_priv;
  if( err ) error(err);
  return ret;
}

// create a new privilege 'name'
nomask string
create_priv(string name)
{
  if(origin() != ORIGIN_LOCAL && name[0] != '@') return 0;

  name = lower_case(name);

  if( privs[name] || !check_previous_priv(get_control_priv(name)) )
    return 0;

  privs[name] = ({ });
  return name;
}

// delete privilege 'name'
nomask int
delete_priv(string name)
{
  if( !privs[name] || !check_previous_priv(get_control_priv(name)) )
    return 0;

  map_delete(privs, name);
  return 1;
}

// open priv1 to priv2
nomask int
open_priv(string priv1, string priv2)
{
  if( !privs[priv1] ||
      !check_previous_priv(get_control_priv(priv1)) ||
      is_opened(priv1, priv2) ) return 0;

  privs[priv1] += ({ priv2 });
}

nomask string*
get_wizards()
{
  return filter(filter(keys(privs), (: strsrch($1, ":") == -1 :)),
		(: $1 == lower_case($1) :));
}

nomask int
wizard_exists(string name)
{
  return (member_array(name, get_wizards()) != -1);
}

nomask string
create_wizard(string name)
{
  name = lower_case(name);

  if( !check_previous_priv(1) || wizard_exists(name) ) return 0;

  privs[name] = ({ });
  privs[name+":"] = ({ });
  return name;
}

nomask int
delete_wizard(string name)
{
  if( !check_previous_priv(1) || !wizard_exists(name) ) return 0;

  // delete associated privileges
  foreach(string priv in get_subprivs(name))
    map_delete(privs, priv);
  return 1;
}

nomask int
domain_exists(string name)
{
  return (domains[name] ? 1 : 0);
}

nomask string
create_domain(string name)
{
  string cap_name;
  name = lower_case(name);

  if( !check_previous_priv(1) || domains[name] ) return 0;

  cap_name = capitalize(name);
  domains[name] = ([ ]);
  privs[cap_name] = ({ });
  privs[cap_name+":"] = ({ });

  return name;
}

nomask int
delete_domain(string name)
{
  if( !check_previous_priv(1) || !domains[name] ) return 0;

  map_delete(domains, name);
  // delete associated subprivileges
  foreach(string priv in get_subprivs(name))
    map_delete(privs, priv);
  return 1;
}

nomask int
admin_domain(string domain, string name)
{
  return domains[domain][name] == DOMAIN_ADMIN;
}

nomask int
member_domain(string domain, string name)
{
  return domains[domain][name] == DOMAIN_MEMBER;
}

nomask int
add_domain_member(string domain, string member)
{
  if( !domains[domain] || !wizard_exists(member) ||
      !check_previous_priv(capitalize(domain)) ) return 0;

  domains[domain][member] = DOMAIN_MEMBER;
  return 1;
}

nomask int
remove_domain_member(string domain, string member)
{
  if( !domains[domain] || !member_domain(domain, member) ||
      !check_previous_priv(capitalize(domain)) ) return 0;


  map_delete(domains[domain], member);
  return 1;
}

nomask int
set_domain_level(string domain, string member, int level)
{
  if( !domains[domain] || !member_domain(domain, member) ||
      !check_previous_priv(capitalize(domain)) ) return 0;

  domains[domain][member] = (level ? DOMAIN_ADMIN : DOMAIN_MEMBER);
  return 1;
}
