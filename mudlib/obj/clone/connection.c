/*  -*- LPC -*-  */
// initial connection for logging in
#pragma no_inherit

#include <mudlib.h>
#include <login.h>
#include <daemons.h>
#include <uid.h>

inherit OBJECT;

private string name;             // username
private string password;         // password, crypted
private string real_name;        // user's real name
private string email_addr;       // user's email address
private string home_dir;         // user's home directory
private string body;             // user's body file
private int    login_time;       // time() when user logged in
private string login_from;       // ip_name from which user connected
private boolean active;          // TRUE if this is an active connection ob

// override of object::remove

int
remove()
{
  if(active)
    return -1;
  else
    return 1;
}

void
net_dead()
{
  destruct(this_object());
}

void
logon()
{
  LOGIN_D->logon();
}

int
save_connection(string username)
{
  string dir;

  dir = CONNECTION_DIR "/" + username[0..0];
  if(file_size(dir) != -2) mkdir(dir);

  return save_object(dir + "/" + username, 1);
}

int
restore_connection(string username)
{
  return restore_object(CONNECTION_DIR "/" +
		     username[0..0] + "/" + username, 1);
}

void
receive_message(string msgClass, string msg)
{
  receive(msg);
}

string query_name() { return name; }
string query_cap_name() { return capitalize(name); }
string query_real_name() { return real_name; }
string query_email_addr() { return email_addr; }
string query_password() { return password; }
string query_home_dir() { return home_dir; }
string query_body() { return body; }
int    query_login_time() { return login_time; }
string query_login_from() { return login_from; }

void
set_active()
{
  active == TRUE;
}

int
set_name(string arg)
{
  if(geteuid(previous_object()) != ROOT_UID)
    return 0;

  name = arg;
  set_ids(({ "connection-"+name }));
  return 1;
}

int
set_password(string arg)
{
  if(geteuid(previous_object()) != ROOT_UID)
    return 0;

  password = arg;
  return 1;
}

int
set_real_name(string arg)
{
  real_name = arg;
  return 1;
}

int
set_email_addr(string arg)
{
  email_addr = arg;
  return 1;
}

int
set_home_dir(string arg)
{
  home_dir = arg;
  return 1;
}

int
set_body(string arg)
{
  body = arg;
  return 1;
}

// also sets 'login_from' at the same time

int
set_login_time(int arg)
{
  login_time = arg;
  login_from = query_ip_name(this_object());
  return 1;
}
