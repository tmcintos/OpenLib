/*  -*- LPC -*-  */
#include <command.h>

// This command changes finger information (found in connection object) for
// a user.  If no username is passed, this_player() is assumed.  If a username
// is passed the action taken depends whether or not the player is logged in.

int
main(string uname)
{
  object conn, user;

  if(!uname) {
    uname = this_player()->query_name();
    conn = this_player()->query_connection();
  } else {
    if(!archp(this_player()))
      return notify_fail("chfn: You must be an archwizard to change another"
			 " user's finger info.\n");

    uname = lower_case(uname);

    if(!user_exists(uname))
      return notify_fail(uname +": no such user.\n");

    if(!user = find_player(uname)) {
      conn = new(CONNECTION_OB);
      conn->restore_connection(uname);
    } else {
      conn = user->query_connection();
    }
  }

  printf("Real Name [%s]: ", (string) conn->query_real_name());
  input_to("get_rn", 0, conn);
  return 1;
}

void
get_rn(string input, object conn)
{
  if(input && input != "")
    conn->set_real_name(input);

  printf("Shell [%s]: ", (string) conn->query_login_shell());
  input_to("get_shell", 0, conn);
}

void
get_shell(string input, object conn)
{
  if(input && input != "") {
    if(!file_exists(input+".c") && !file_exists(input)) {
      printf("No such file: %s, please re-enter.\n", input);
      return get_rn(0, conn);
    } else
      conn->set_login_shell(input);
  }

  printf("Body Object [%s]: ", (string) conn->query_body());
  input_to("get_body", 0, conn);
}

void
get_body(string input, object conn)
{
  if(input && input != "") {
    if(!file_exists(input+".c") && !file_exists(input)) {
      printf("No such file: %s, please re-enter.\n", input);
      return get_shell(0, conn);
    } else
      conn->set_body(input);
  }

  printf("Email Address [%s]: ", (string) conn->query_email_addr());
  input_to("get_email", 0, conn);
}

void
get_email(string input, object conn)
{
  if(input && input != "")
    conn->set_email_addr(input);

  conn->save_connection((string) conn->query_name());

  // This won't be able to dest a player's connection which is good
  // remove() in active connections returns -1
  destruct(conn);

  printf("Finger information changed.\n");
}

string
help_desc()
{
  return "change finger information for a user";
}

string
help()
{
  return @ENDHELP
usage:  chfn [username]

If username is given, change finger information for that user.  Otherwise
assume this player.  Note that you must be an admin to change finger
information for another player.
ENDHELP;

}
