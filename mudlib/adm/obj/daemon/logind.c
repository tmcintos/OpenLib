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
//    01/20/96                 Fixed mud-locking by 'all' group feature

#include <mudlib.h>
#include <login.h>      // login related defines
#include <daemons.h>    // for groupd
#include <uid.h>        // for group ids

#define DEBUG
#define display(x) message("system", x, this_player());

inherit DAEMON;

mapping attempt_number;       // mapping of # login fails for each connection

/* Prototypes */
// only call these externally
int check_passwd(string username, string password);
void logon();

// don't call these externally
void create();
string lookup_password(string username);
void get_uname();
void logon2(string username);
void get_password(string password);
varargs void get_new_password(string password, int verify, string lastpass);
void ask_throw_out(string input, object other_player);
void player_enter_world(int new_char);
void player_enter_world2(string user, object body);
int valid_name(string username);
void dump_connection(object c);

#ifdef DEBUG
mapping
dump_attempt_number()
{
  return copy(attempt_number);
}
#endif

void
create()
{
  daemon::create();
  attempt_number = ([]);
}

// use check_password() publicly, not this

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

// Use this to get rid of a connected user

void
dump_connection(object c)
{
  map_delete(attempt_number, c);
  efun::destruct(c);
}

/*-------------------------- Login Sequence ---------------------------------*/

void
logon()
{
  cat(ISSUE);
  display("\n");
  printf("Mudlib: %s v%s\n", mudlib_name(), mudlib_version());
  printf("Driver: %s (%s)\n", driver_version(), machine_os());

  attempt_number += ([ this_player() : 0 ]);

  get_uname();
}

void
get_uname()
{
  object conn = this_player();

  attempt_number[conn]++;

  if(attempt_number[conn] > 4) {
    display("repeated login failures.\n");
    dump_connection(conn);
  } else {
    display("login: ");
    input_to("logon2");
  }
}

void
logon2(string username)
{
  object conn = this_player();
  string pw;

  username = lower_case(username);

  if(username == "quit") {
    dump_connection(conn);
    return;
  }

//  This is for mudlocking; simply make group (ALL_GID) the users allowed in

  if(!GROUP_D->is_member(ALL_GID, username)) {
    display("Sorry, you are not allowed to log in right now.\n");
    return get_uname();
  }

  if(!conn->restore_connection(username)) {
    if(!valid_name(username)) return get_uname();
    conn->set_name(username);

    display("New Character.\n");
    display("You must now enter a password for your character.\n");
    display("Password: ");
    input_to("get_new_password", 1, FALSE);
    return;
  } else if(pw == "") {
    display("You have no password!!\n");
    player_enter_world(FALSE);
    return;
  } else {
    display("Password: ");
    input_to("get_password", 1);
  }
}

void
get_password(string password)
{
  object conn = this_player();

  if(!check_passwd((string)conn->query_name(), password)) {
    display("\nlogin incorrect.\n");
    return get_uname();
  }

  player_enter_world(FALSE);
}

varargs
void
get_new_password(string password, int verify, string lastpass)
{
  if (!verify) {
    display("Again to verify: ");
    input_to("get_new_password", 1, TRUE, password);
    return;
  } else if (password != lastpass) {
    display("They don't match!!\n");
    return get_uname();
  }

// set name before, now set password
  this_player()->set_password(crypt(password, 0));

  player_enter_world(TRUE);
}

void
ask_throw_out(string input, object other_player)
{
  if(!input || (input[0] != 'y' && input[0] != 'n')) {
    display("Please answer y or n: ");
    input_to("ask_throw_out", other_player);
  } else {
    switch(input[0]) {
    case 'y':
      message("system", "You're outta here!\n", other_player);
      destruct(other_player);
      player_enter_world(FALSE);   // should never happen w/a new character
      break;
    case 'n':
      display("Ok, have it your way...\n");
      dump_connection(this_player());
      break;
    }
  }
}

void
player_enter_world(int new_char)
{
  object body;
  object conn = this_player();
  string username = (string) conn->query_name();
  string player_body, buf = "";

  display("\n");

// If linkdead, reconnect
  if(body = find_player(username)) {
    if(interactive(body)) {
      display("You are already playing!\n");
      display("Throw out other copy? [y/n]: ");
      input_to("ask_throw_out", body);
    } else {
      if(body->query_connection())            // remove old connection ob
	dump_connection(body->query_connection());
      body->set_connection(conn);
      exec(body, conn);
      body->reconnect();
    }
    return;
  }

  map_delete(attempt_number, conn);           // delete us from login fail map

  if(player_body = (string)conn->query_body())
    body = new(player_body);

  if(!body) {
    log_file("logind", "failed to load body for " + username +"\n");
    buf += "!!! Failed to load your body!!  Loading default body.\n"
              "!!! Please notify an Admin.\n";
    body = new(USER_OB);
  }

  if(!conn->query_home_dir())
    conn->set_home_dir(user_cwd(username));   // set up default home dir

  conn->set_login_time(time());
  conn->save_connection(username);

  body->set_connection(conn);
  exec(body, conn);

  // set eff uid, then export it to user; MUST BE DONE BEFORE setup()!!!
  seteuid(username);
  if(!export_uid(body))
    buf += "logind: failed to set uid!!\n";
  seteuid(getuid(this_object()));                   // reset logind's euid

  body->setup(username);
  if(new_char)
    body->save_player();

  buf += "Welcome to " + mud_name() + "!\n\n";
  buf += read_file(MOTD);
  buf += "\n\n";

  if(MAIL_D->check_mail(username))
    buf += sprintf("You have %smail.",
		      (MAIL_D->check_mail(username, 1) ? "NEW " : ""));

  body->more(explode(buf, "\n"), (: player_enter_world2, username, body :));
}

void
player_enter_world2(string username, object body)
{
  tell_room(START_ROOM, sprintf("%s enters this reality.\n",
				(string)body->query_cap_name()));
  body->move(START_ROOM);
  body->force_me("look");

  write_file(LASTLOG, sprintf("%-10.10s  %-20.20s  %s  %-15.15s\n",
				     username,
				     query_ip_name(body),
				     ctime(time()),
				     query_ip_number(body)));
}

/*-------------------------- Name & Password Checks -------------------------*/

int
valid_name(string username)
{
  int i;

  if(username == "" || !username)
    return 0;

  if(strlen(username) > 10 || strlen(username) < 3) {
    display("Username must be between 3 and 10 characters in length.\n");
    return 0;                                         // Too long or short
  }

  for(i=0;i < strlen(username);i++)                   // Name has special chars
    if(username[i] < 'A' ||
       username[i] > 'z' ||
       (username[i] > 'Z' && username[i] < 'a')) {
      display("Username must not consist of letters only.\n");
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
