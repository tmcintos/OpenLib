/*  -*- LPC -*-  */
//  logind.c:  handles logins to the mud
//
//  Orig written by Tim the regional LPC guru.
//
//  CHANGES:
//    05/05/95  Tim McIntosh:  changed minor details
//    05/06/95                 split into logon and logon2; put in name support
//    08/26/95                 put in password type stuff
//    somedate                 <did a major rewrite>
//    09/14/95                 put public definitions into logind.h
//    sometime                 <another major rewrite...no more logind.h>

#include <mudlib.h>
#include <login.h>      // login related defines
#include <daemons.h>    // for groupd
#include <uid.h>        // for group ids

inherit DAEMON;

mapping attempt_number;

// only call this externally
int check_passwd(string username, string password);
void logon();

/* Prototypes */
void create();
string lookup_password(string username);
void get_uname(object conn);
void logon2(string username, object conn);
void get_password(string password, object conn);
varargs void get_new_password(string password, int verify, object conn,
			      string lastpass);
void player_enter_world(object conn, int new_char);
int valid_name(string username);

void
create()
{
  daemon::create();
  SetNoClean(1);  // don't clean up
  attempt_number = ([]);
}

// use check password publicly, not this

string
lookup_password(string username)
{
  string ret;
  object conn = new(CONNECTION_OB);

  conn->restore_connection(username);
  ret = (string)conn->query_password();

  destruct(conn);
  return ret;
}

/*-------------------------- Login Sequence ---------------------------------*/

void
logon()
{
  object conn;
  conn = this_player();

  cat(ISSUE);
  write("\nDriver: "+ __VERSION__ +" ("+ __ARCH__ +")\n");

  attempt_number += ([ conn : 0 ]);

  get_uname(conn);
}

void
get_uname(object conn)
{
  attempt_number[conn]++;

  if(attempt_number[conn] > 4) {
    write("repeated login failures.\n");
    destruct(conn);
    return;
  }
  
  write("login: ");
  input_to("logon2", 0, conn);
}

void
logon2(string username, object conn)
{
  string pw;

  username = lower_case(username);

  if(username == "quit") {
    destruct(conn);
    return;
  }

  if(!conn->restore_connection(username)) {
    if(!valid_name(username)) return get_uname(conn);
    conn->set_name(username);

    write("New Character.\n");
    write("You must now enter a password for your character.\n");
    write("Password: ");
    input_to("get_new_password", 1, FALSE, conn);
    return;
  } else if(pw == "") {
    write("You have no password!!\n");
    player_enter_world(conn, FALSE);
    return;
  } else {
    write("Password: ");
    input_to("get_password", 1, conn);
  }
}

void
get_password(string password, object conn)
{

  if(!check_passwd((string)conn->query_name(), password)) {
    write("\nlogin incorrect.\n");
    return get_uname(conn);
  }

  player_enter_world(conn, FALSE);
}

varargs void
get_new_password(string password, int verify, object conn, string lastpass)
{
  if (!verify) {
    write("Again to verify: ");
    input_to("get_new_password", 1, TRUE, conn, password);
    return;
  } else if (password != lastpass) {
    write("They don't match!!\n");
    return get_uname(conn);
  }

// set name before, now set password
  conn->set_password(crypt(password, 0));

  player_enter_world(conn, TRUE);
}

void
player_enter_world(object conn, int new_char)
{
  string player_body = 0;
  string username = (string)conn->query_name();
  object body;

  write("\n");
//  If linkdead, reconnect
  if(body = find_player(username)) {
    if(interactive(body)) {
      write("You are already playing!  Sorry.\n");
      destruct(conn);
      return;
    }
    if(body->query_connection())              // remove old connection ob
      efun::destruct(body->query_connection());
    body->set_connection(conn);
    exec(body, conn);
    conn->move(body);
    body->reconnect();
    return;
  }

  if(player_body = (string)conn->query_body()) {
    body = new((string)conn->query_body());
  }

  if(!body) {
    log_file("logind", "failed to load body for " + username +"\n");
    write("!!! Failed to load your body!!  Loading default body.\n"
	  "!!! Please notify an Admin.\n");
    body = new(USER_OB);
  }

  if(!conn->query_home_dir())
    conn->set_home_dir(user_cwd(username));
  conn->set_login_time(time());

  conn->save_connection(username);
  body->set_connection(conn);
  conn->move(body);            // move into body
  exec(body, conn);

  if(new_char) {
    body->set_name(username);
    body->save_player();
  }

  // set eff uid, then export it to user; MUST BE DONE BEFORE setup()!!!
  seteuid(username);
  if(!export_uid(body))
    tell_object(body, "logind: failed to set uid!!\n");
  seteuid(getuid(this_object()));                   // reset our euid

  body->setup(username);
  write("Welcome to "+ mud_name() +"\n\n");
  cat(MOTD);

  tell_room(START_ROOM, (string)body->query_cap_name() +
	    " enters this reality.\n");

  body->move(START_ROOM);
}

/*-------------------------- Name & Password Checks -------------------------*/

int
valid_name(string username)
{
  int i;

//  This is for mudlocking; simply make group (ALL_GID) the users allowed in

  if(!GROUP_D->is_member(ALL_GID, username)) {
    write("Sorry, you are not allowed to log in right now.\n");
    return 0;
  }

  if(username == "" || !username)
    return 0;

  if(strlen(username) > 10 || strlen(username) < 3) {
    write("Username must be between 3 and 10 characters in length.\n");
    return 0;                                         // Too long or short
  }

  for(i=0;i < strlen(username);i++)                   // Name has special chars
    if(username[i] < 'A' ||
       username[i] > 'z' ||
       (username[i] > 'Z' && username[i] < 'a')) {
      write("Username must not consist of letters only.\n");
      return 0;
    }
  return 1;                                           // Name is OK
}

int
check_passwd(string username, string passwd)
{
  string crypted_pass;

  crypted_pass = lookup_password(username);

  if(!crypted_pass) return 0;

  if(crypt(passwd, crypted_pass[0..1]) == crypted_pass) return 1;
  return 0;
}
