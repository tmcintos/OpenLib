/*  -*- LPC -*-  */
// auth.c:  Intermud3 'auth' service implementation
//          performs mud authentication
//
// created 12.8.95 by Tim@Dysfunctional Mud

#include <daemons.h>
#include <net/daemons.h>

#define SERVICE_AUTH

void
eventRecieveAuthRequest(mixed *packet)
{
  int key;

  if( file_name(previous_object()) != INTERMUD_D )
    return;

  key = AUTH_D->generate_key(packet[2]);

  INTERMUD_D->eventWrite( ({
    "auth-mud-reply",
    5,
    mud_name(),
    0,
    packet[2], /* target mud */
    0,
    key }) );
}

void
eventRecieveAuthReply(mixed *packet)
{
  if( file_name(previous_object()) != INTERMUD_D )
    return;

  AUTH_D->store_key(packet[2] /* mud */, packet[6] /* key */);
}

void
eventSendAuthRequest(string target_mudname)
{
//  if( file_name(preivous_object()) != AUTH_D )
//    return;

  INTERMUD_D->eventWrite( ({
    "auth-mud-req",
    5,
    mud_name(),
    0,
    target_mudname,
    0 }) );
}
