/*  -*- LPC -*-  */
#include <daemons.h>
#include <security.h>

// The unguarded function: evaluate 'code' at privilege level 'priv'

varargs mixed
unguarded(function code, mixed priv)
{
  return SECURITY_D->eval_unguarded(code, priv);
}

// Return a priv as a printable string

string
sprint_priv(mixed priv)
{
  if( stringp(priv) )
    return priv;
  else
    return sprintf("%i", priv);
}

// Useful for checking everything

int
check_priv(mixed priv)
{
  return SECURITY_D->check_privilege(priv, 1);
  // first object on stack will be simul_efun (security_d is taken off always)
}

// Useful when you know this_object() is secure..checks everything else

int
check_previous_priv(mixed priv)
{
  return SECURITY_D->check_privilege(priv, 2);
  // first object on stack will be simul_efun (security_d is taken off always)
}

mixed
query_privs(object ob)
{
  mixed priv = restore_variable(efun::query_privs(ob));

  return priv;
}

int
set_privs(object ob, mixed priv)
{
  if( !check_previous_priv(priv) ) return 0;

  efun::set_privs(ob, save_variable(priv));
  return 1;
}

int
admin_exists(string a)
{
  if(!a) return 0;
  return SECURITY_D->member_domain(ADMIN_DOMAIN, a);
}

int
wizard_exists(string w)
{
  if(!w) return 0;
  return SECURITY_D->wizard_exists(w);
}

int
domain_exists(string d)
{
  if(!d) return 0;
  return SECURITY_D->domain_exists(d);
}

int
member_domain(string name, string domain)
{
  return SECURITY_D->member_domain(domain, name);
}

int
admin_domain(string name, string domain)
{
  return SECURITY_D->admin_domain(domain, name);
}

/*
 * Functions for checking status of objects
 */

int
visitorp(object ob)
{
  return 0;
}

int
wizardp(object ob)
{
  string name = ob->query_name();

  if( name )
    return SECURITY_D->wizard_exists(name);
  else
    return 0;
}

int
adminp(object ob)
{
  string name = ob->query_name();

  if( name )
    return SECURITY_D->member_domain(ADMIN_DOMAIN, name);
  else
    return 0;
}

// this is tagged for removal -- Tim
int
archp(object ob)
{
// same here :>
  return 1;
}
