/*  -*- LPC -*-  */
// initial connection for logging in
// Written by Tim sometime in fall 1995

#include <mudlib.h>
#include <login.h>
#include <daemons.h>

private string name;             // username
private string cap_name;         // capitalized version of username
private string password;         // password, crypted
private string real_name;        // user's real name
private string email_addr;       // user's email address
private string body;             // user's body file
private string login_shell;      // user's login shell file
private int    gender;           // gender of user
private int    login_time;       // time() when user logged in
private int    logout_time;      // time() when user quit
private string login_from;       // ip_name from which user connected

static boolean is_active;        // active if logon() has been called

static string  ttype;            // terminal type
static int     width;            // terminal width
static int     height;           // terminal height

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

void
window_size(int w, int h)
{
  width = w;
  height = h;
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
int    query_gender() { return gender; }
string query_login_shell() { return login_shell; }
int    query_login_time() { return login_time; }
int    query_logout_time() { return logout_time; }
string query_login_from() { return login_from; }
mixed* query_term_info() { return ({ ttype, width, height }); }

int
set_name(string arg)
{
  /* security -- admin level */

  name = arg;
  return 1;
}

int
set_cap_name(string arg)
{
  /* security -- admin level */

  if(lower_case(arg) == name) {
    cap_name = arg;
    return 1;
  }

  return 0;
}

int
set_password(string arg)
{
  /* security -- admin level */

  password = arg;
  return 1;
}

int
set_real_name(string arg)
{
  /* security -- admin level */

  real_name = arg;
  return 1;
}

int
set_email_addr(string arg)
{
  /* security -- admin level */

  email_addr = arg;
  return 1;
}

int
set_body(string arg)
{
  /* security -- admin level */

  body = arg;
  return 1;
}

int
set_gender(int arg)
{
  /* security -- admin level */

  gender = arg;
  return 1;
}

int
set_login_shell(string arg)
{
  /* security -- admin level */

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
