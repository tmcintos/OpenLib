/*  -*- LPC -*-  */
#include <command.h>

void loop(string input, object client);
void eventRead(string val, object ob);
void eventSocketClosed(object ob, object client);

int
main()
{
  object client = new("/u/t/tim/bin/socketio");
  client->SetRead((: eventRead, this_player() :));
  client->SetSocketClose((: eventSocketClosed, this_player() :));
  if(client->eventCreateSocket(1, " ") < 0) {
    message("system", "CreateSocket failed.\n", this_player());
    return 1;
  }

  input_to( (:loop:), client );
  return 1;
}

void
loop(string input, object client)
{
  client->eventWrite(input);
  input_to( (:loop:), client );
}

void
eventRead(object ob, string val)
{
  message("system", val, ob);
}

void
eventSocketClosed(object ob, object client)
{
  message("system", "Socket Closed.\n", ob);
  destruct(client);
}
