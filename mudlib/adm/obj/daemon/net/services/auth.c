/*  -*- LPC -*-  */
// auth.c:  Intermud3 'auth' service implementation
//          performs mud authentication
//          Also sends/receives oob-req packets
//
// created 12.8.95 by Tim@Dysfunctional Mud
// 3.23.96  Tim  Added the oob-req stuff

#include <net/daemons.h>

#define SERVICE_AUTH

void
eventReceiveOOBRequest(mixed *packet)
{
  if( file_name(previous_object()) != INTERMUD_D ) return;

  AUTH_D->generate_key(packet[2]);
  OOB_D->open_oob_port();
}

void
eventSendOOBRequest(string target_mudname)
{
  INTERMUD_D->eventWrite( ({
    "oob-req",
    5,
    mud_name(),
    0,
    target_mudname,
    0 }) );
}
  
void
eventReceiveAuthRequest(mixed *packet)
{
  int key;

  if( file_name(previous_object()) != INTERMUD_D ) return;

  key = AUTH_D->generate_key(packet[2]);
  OOB_D->open_oob_port();

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
eventReceiveAuthReply(mixed *packet)
{
  if( file_name(previous_object()) != INTERMUD_D ) return;

  AUTH_D->store_key(packet[2] /* mud */, packet[6] /* key */);
}

void
eventSendAuthRequest(string target_mudname)
{
//  if( file_name(preivous_object()) != OOB_D )
//    return;

  INTERMUD_D->eventWrite( ({
    "auth-mud-req",
    5,
    mud_name(),
    0,
    target_mudname,
    0 }) );
}
