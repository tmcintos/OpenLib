/*  -*- LPC -*-  */
#include <command.h>

int
main()
{
  object conn;

  conn = new(CONNECTION_OB);
  conn->restore_connection((string) this_player()->query_name());

  printf("Real Name [%s]: ", (string) conn->query_real_name());
  input_to("get_rn", 0, conn);
  return 1;
}

void
get_rn(string input, object conn)
{
  if(input && input != "")
    conn->set_real_name(input);

  printf("Body Object [%s]: ", (string) conn->query_body());
  input_to("get_body", 0, conn);
}

void
get_body(string input, object conn)
{
  if(input && input != "")
    conn->set_body(input);

  printf("Email Address [%s]: ", (string) conn->query_email_addr());
  input_to("get_email", 0, conn);
}

void
get_email(string input, object conn)
{
  if(input && input != "")
    conn->set_email_addr(input);

  conn->save_connection((string) conn->query_name());
  destruct(conn);

  printf("Finger information changed.\n");
}
