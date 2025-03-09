//  -*- LPC -*-
//  login_d.c:  handles logins to the mud
//
// Copyright (C) 1996 Tim McIntosh (tmcintos@dm.imaginary.com)
//
// This program is part of the OpenLib Mudlib distribution; it
// is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published
// by the Free Software Foundation; either version 2 of the License,
// or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// If you acquired this program as part of the OpenLib Mudlib
// distribution, you should have received a copy of the GNU
// General Public License in the file /doc/GPL; if not,
// write to the Free Software Foundation, Inc., 675 Mass Ave,
// Cambridge, MA 02139, USA.
//
// Orig written by Tim the regional LPC guru.
//
//  FEATURES:
//    Checks for the existence of the following files to enable features:
//      CONFIG_DIR "/adminlock"      Locks out all but admins.
//      CONFIG_DIR "/wizlock"        Locks out all but wizards.
//      CONFIG_DIR "/mudlock"        Prevents the autocreation of new users.
//
//  CHANGES:
//    05/05/95  Tim McIntosh:  changed minor details
//    05/06/95                 split into logon and logon2; put in name support
//    08/26/95                 put in password type stuff
//    somedate                 <did a major rewrite>
//    09/14/95                 put public definitions into logind.h
//    sometime                 <another major rewrite...no more logind.h>
//    01/20/96                 Fixed mud-locking by 'all' group feature
//    03/01/96                 Cleaned up some more
//    04/16/96                 Support for setting window size automatically
//    08/24/96                 Added mudlock feature to prevent new users.
//    09/03/96                 Changed the way TERM is set.
//    09/22/96                 Cleaned up code a little
//

#include <mudlib.h>
#include <daemons.h>    // for groupd
#include <login.h>      // login related defines
#include <origin.h>
#include <gender.h>

#define DEBUG
#define display(x) message("system", x, this_player())

inherit DAEMON;

private mapping attempt_number; // mapping of # login fails for each connection

/*
 * Public Functions -- only call check_password unless you know what yer doin
 */
public void create();
public void logon(int silent);
public int check_passwd(string username, string password);
public void dump_connection(object c);
public int valid_name(string username);

/*
 * Private Functions
 */
private string lookup_password(string username);
private void get_uname();
private void logon2(string username);
private void get_password(string password);
private varargs void get_new_password(string password, int verify, string lastpass);
private void prompt_cap_name(int try);
private void get_cap_name(string input, int try);
private void get_player_info(string input, int which);
private void ask_throw_out(string input, object other_player);
private void player_enter_world(int new_char);
private void player_enter_world2(string user, object body);

#ifdef DEBUG
mapping
dump_attempt_number()
{
  return copy(attempt_number);
}
#endif

public
void
create()
{
  ::create();
  attempt_number = ([]);
}

// use check_password() publicly, not this

private
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
  if( origin() != ORIGIN_LOCAL && previous_object() != c ) return 0;

  map_delete(attempt_number, c);
  destruct(c, TRUE);
}

/*-------------------------- Login Sequence ---------------------------------*/

void
logon(int silent)
{
  if( nullp(silent) )
  {
    display(sprintf("%s\n", read_file(ISSUE)));
    display(sprintf("Mudlib: %s v%s     ", mudlib_name(), mudlib_version()));
    display(sprintf("Driver: %s (%s)\n", driver_version(), machine_os()));
  }
  attempt_number += ([ this_player() : 0 ]);
  get_uname();
}

private
void
get_uname()
{
  object conn = this_player();

  attempt_number[conn]++;

  if( attempt_number[conn] > 4 )
  {
    display("repeated login failures.\n");
    dump_connection(conn);
  }
  else
  {
    display(PROMPT_LOGIN);
    input_to("logon2");
  }
}

private
void
logon2(string username)
{
  object conn = this_player();
  string pw;
  int t;

  username = lower_case(username);

  if( !sizeof(username) )
  {
    display("Invalid username.\n");
    return get_uname();
  }
  
  if( username == "quit" )
  {
    dump_connection(conn);
    return;
  }

  if( t = BANISH_D->query_banned(username) )
  {
    display(sprintf("%s has been banished from %s%s.  Sorry!\n",
		    capitalize(username),
		    mud_name(),
		    (t == -1 ? "" : " until " + ctime(t))));
    return get_uname();
  }

  if( file_exists(CONFIG_DIR "/adminlock") && !admin_exists(username) )
  {
    display("Only administrators may login right now.\n");
    return get_uname();
  }

  if( file_exists(CONFIG_DIR "/wizlock") && !wizard_exists(username) )
  {
    display("Only wizards may login right now.\n");
    return get_uname();
  }

  if( file_exists(CONFIG_DIR "/mudlock") && !user_exists(username) )
  {
    display("No new characters may be created right now.\n");
    return get_uname();
  }

  if( !conn->restore_connection(username) )
  {
    if(!valid_name(username))
      return get_uname();
    conn->set_name(username);
    display("New Character.\n");
    display("You must now enter a password for your character.\n");
    display("Password: ");
    input_to("get_new_password", 1, FALSE);
  }
  else if( pw == "" )
  {
    display("You have no password!!\n");
    player_enter_world(FALSE);
  }
  else
  {
    display(PROMPT_PASSWORD);
    input_to("get_password", 1);
  }
}

private
void
get_password(string password)
{
  if( !check_passwd((string)this_player()->query_name(), password) )
  {
    display("\nlogin incorrect.\n");
    return get_uname();
  }
  player_enter_world(FALSE);
}

private
varargs
void
get_new_password(string password, int verify, string lastpass)
{
  if ( !verify )
  {
    display("Again to verify: ");
    input_to("get_new_password", 1, TRUE, password);
    return;
  }
  else if( password != lastpass )
  {
    display("They don't match!!\n");
    return get_uname();
  }
  // set name before, now set password
  this_player()->set_password(crypt(password, 0));

  display("\n");
  prompt_cap_name(0);
}

private
void
prompt_cap_name(int try)
{
  display("Enter your username capitalized how you want it to appear\nname: ");
  input_to("get_cap_name", 0, try);
}

private
void
get_cap_name(string input, int try)
{
  if( !this_player()->set_cap_name(input) )
  {
    if( try < 4 )
    {
      display("Spelling must be the same as username.\n");
      return prompt_cap_name(try + 1);
    }
    else
      display("Leaving cap-name as default.\n");
  }
  display("Please enter your email address: ");
  input_to("get_player_info", 0, 0);
}

private
void
get_player_info(string input, int which)
{
  switch( which )
  {
  case 0: /* GET EMAIL */
    {
      this_player()->set_email_addr((sizeof(input) ? input : "(none given)"));
      display("Please enter your real name: ");
      which = 1;
      break;
    }
  case 1: /* GET REAL NAME */
    {
      this_player()->set_real_name((sizeof(input) ? input : "(none given)"));
      display("Are you male, female, or neutral? [m/f/n] ");
      which = 2;
      break;
    }
  case 2: /* GET GENDER */
    {
      if( !input )
	input = "";
      switch( input[0] )
      {
      case 'm':
	if( this_player()->set_gender(GENDER_MALE) )
	  return  player_enter_world(TRUE);
      case 'f':
	if( this_player()->set_gender(GENDER_FEMALE) )
	  return  player_enter_world(TRUE);
      case 'n':
	if( this_player()->set_gender(GENDER_NEUTRAL) )
	  return  player_enter_world(TRUE);
      }
      display("Please enter m, f, or n: ");
      which = 2;
      break;
    }
  }
  input_to("get_player_info", 0, which);
}

private
void
ask_throw_out(string input, object other_player)
{
  if( !input || (input[0] != 'y' && input[0] != 'n') )
  {
    display("Please answer y or n: ");
    input_to("ask_throw_out", other_player);
  }
  else
  {
    switch( input[0] )
    {
    case 'y':
      message("system", "You're outta here!\n", other_player);
      destruct(other_player);
      player_enter_world(FALSE);   // should never happen w/a new character
      break;
    case 'n':
      display("Fine then! If that's the way you want it!!\n");
      dump_connection(this_player());
      break;
    }
  }
}

private
void
player_enter_world(int new_char)
{
  object conn = this_player();
  string username = (string) conn->query_name();
  object body, shell;
  string buf = "";

  // delete us from login fail map
  map_delete(attempt_number, conn);
  display("\n");


  // lastlog entry
  write_file(LASTLOG, sprintf("%:-10s  %:-20s  %s  %:-15s\n",
			      username, query_ip_name(conn), ctime(time()),
			      query_ip_number(conn)));

  // If linkdead, reconnect
  if( body = find_player(username) )
  {
    if( interactive(body) )
    {
      display("You are already playing!\n");
      display("Throw out other copy? [y/n]: ");
      input_to("ask_throw_out", body);
    }
    else
    {
      if( body->query_connection() )            // remove old connection ob
	dump_connection(body->query_connection());
      body->set_connection(conn);
      exec(body, conn);
      body->reconnect();
    }
    return;
  }

  // Get user's body
  if( !(string)conn->query_body() )
    conn->set_body( DEFAULT_BODY );
  if( !(body = new( conn->query_body() )) )
  {
    log_file("login_d", "failed to load body for " + username +"\n");
    buf += "!!! Failed to load your body!!  Loading default body.\n"
           "!!! Please notify an Admin.\n";
    body = new( DEFAULT_BODY );               // guaranteed to load *grin*
  }

  // exp!!!
  // setup races
  body->init_race("/obj/daemon/race_d"->query_raceinfo("human"));
  body->init_combat();
  
  // Get user's shell
  if( !conn->query_login_shell() )
    conn->set_login_shell( DEFAULT_SHELL );
  if( !(shell = new( conn->query_login_shell())) )
  {
    log_file("login_d", "failed to load shell for " + username +"\n");
    buf += "!!! Failed to load your shell!!  Loading default shell.\n"
           "!!! Please notify an Admin.\n";
    shell = new( DEFAULT_SHELL );            // also guaranteed to load *grin*
  }
  conn->set_login_time(time());
  conn->save_connection(username);

  body->set_connection(conn);
  exec(body, conn);

  body->init_player(username);
  if( !shell->shell_init(body) )
  {
    buf += "login_d: couldn't init your shell.\n";
    destruct(shell);
  }
  body->set_cap_name(conn->query_cap_name());
  body->set_gender(conn->query_gender());
  if( new_char )
  { 
    string dir = USER_BODY_DIR "/" + username[0..0];

    /* create directory if needed */
    if( file_size(dir) == -1 )
      if( !mkdir(dir) )
      {
	log_file("login_d", "Could not create directory: " + dir + "\n");
	buf += "!!! Couldn't create directory " + dir + 
               "; notify a wizard immediately!\n";
      }
    body->save_player(username);
  }

  /*
   * security -- setup privs
   * NOTE: this must be done after the stuff above, or else the calls to
   * restore/save_object will fail
   */
  if( admin_exists(username) )
    set_privs(body, 1);
  else if( wizard_exists(username) )
    set_privs(body, username);
  else
    set_privs(body, 0);

  if( !body->get_env("NO_AUTO_SET_TERMINFO") )
  {
    mixed* tinfo = conn->query_term_info();

    if( member_array(tinfo[0], TERMCAP_D->query_supported_termtypes()) == -1 )
    {
      string ttype = tinfo[0];
      
      if( !tinfo[0] = body->get_env("DEFAULT_TERM") )  // user set default
	tinfo[0] = DEFAULT_TERM;                       // system default

      buf += sprintf("login_d: '%s' is not a supported terminal type.\n"
		     "         Using '%s' as default terminal type.\n",
		     ttype, tinfo[0]);
    }      
    body->set_env("TERM", tinfo[0]);

    if( tinfo[1] ) body->set_env("WIDTH", tinfo[1]);
    if( tinfo[2] ) body->set_env("LENGTH", tinfo[2]);
  }

  buf += sprintf("Welcome to %s!\n%s\n\n", mud_name(), read_file(MOTD));

  body->more(explode(buf, "\n"), (: player_enter_world2, username, body :));
}

private
void
player_enter_world2(string username, object body)
{
  string lf = user_path(username) + ".login";
  
  tell_room(START_ROOM, sprintf("%s enters this reality.\n",
				(string)body->query_cap_name()));
  body->move(START_ROOM);

  if( wizard_exists(username) && file_exists(lf) )
    body->force_me("source " + lf);
  else
    body->force_me("look");

  if( MAIL_D->check_mail(username) ) {
    if( MAIL_D->check_mail(username, 1) )
      display("\nYou have %^GREEN%^NEW%^RESET%^ mail.\n\n");
    else
      display("You have mail.\n");
  }
}

/*-------------------------- Name & Password Checks -------------------------*/

public
int
valid_name(string username)
{
  if( username == "" || nullp(username) )
    return 0;

  if( strlen(username) > MAX_USERNAME_LENGTH ||
      strlen(username) < MIN_USERNAME_LENGTH)
  {
    display("Username must be between 3 and 10 characters in length.\n");
    return 0;                                        // Too long or short
  }

  for( int i = 0; i < sizeof(username); i++ )        // Name has special chars
  {
    if( username[i] < 'A' || username[i] > 'z' ||
        (username[i] > 'Z' && username[i] < 'a') )
    {
      display("Username must consist of alphabetic characters only.\n");
      return 0;
    }
  }

  return 1;                                           // Name is OK
}

public
int
check_passwd(string username, string passwd)
{
  string crypted_pass = lookup_password(username);

  if( !crypted_pass )
    return 0;
  else
    return ( crypt(passwd, crypted_pass[0..1]) == crypted_pass );
}
