/*  -*- LPC -*-  */
// initial connection for logging in
// Written by Tim sometime in fall 1995

#include <mudlib.h>
#include <login.h>
#include <daemons.h>
#include <uid.h>

private string name;             // username
private string cap_name;         // capitalized version of username
private string password;         // password, crypted
private string real_name;        // user's real name
private string email_addr;       // user's email address
private string body;             // user's body file
private string login_shell;      // user's login shell file
private int    login_time;       // time() when user logged in
private int    logout_time;      // time() when user quit
private string login_from;       // ip_name from which user connected

static boolean is_active;        // active if logon() has been called
static string  ttype;            // terminal type

// override of object::remove
int
remove()
{
  if(is_active)
    return 1;
  else
    return 0;
}

void
net_dead()
{
  LOGIN_D->dump_connection(this_object());
}

void
logon()
{
  is_active = 1;
  log_file("connect", query_ip_name(this_object()) + "\n");
  LOGIN_D->logon();
}

void
terminal_type(string term)
{
  ttype = lower_case(term);
  log_file("connect", sprintf("ttype: %s\n", ttype));
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
string query_cap_name() { return cap_name; }
string query_real_name() { return real_name; }
string query_email_addr() { return email_addr; }
string query_password() { return password; }
string query_body() { return body; }
string query_login_shell() { return login_shell; }
int    query_login_time() { return login_time; }
int    query_logout_time() { return logout_time; }
string query_login_from() { return login_from; }
string query_terminal_type() { return ttype; }

int
set_name(string arg)
{
  if( geteuid(previous_object()) != ROOT_UID ) return 0;

  name = arg;
  return 1;
}

int
set_cap_name(string arg)
{
  if( geteuid(previous_object()) != ROOT_UID ) return 0;

  if(lower_case(arg) == name) {
    cap_name = arg;
    return 1;
  }

  return 0;
}

int
set_password(string arg)
{
  if( geteuid(previous_object()) != ROOT_UID ) return 0;

  password = arg;
  return 1;
}

int
set_real_name(string arg)
{
  if( geteuid(previous_object()) != ROOT_UID ) return 0;

  real_name = arg;
  return 1;
}

int
set_email_addr(string arg)
{
  if( geteuid(previous_object()) != ROOT_UID ) return 0;

  email_addr = arg;
  return 1;
}

int
set_body(string arg)
{
  if( geteuid(previous_object()) != ROOT_UID ) return 0;

  body = arg;
  return 1;
}

int
set_login_shell(string arg)
{
  if( geteuid(previous_object()) != ROOT_UID ) return 0;

  login_shell = arg;
  return 1;
}

// also sets 'login_from' at the same time

int
set_login_time(int arg)
{
  if( file_name(previous_object()) != LOGIN_D ) return 0;

  login_time = arg;
  login_from = query_ip_name(this_object());
  return 1;
}

int
set_logout_time(int arg)
{
  logout_time = arg;
  return 1;
}
