/*  -*- LPC -*-  */
#include <command.h>
#include <daemons.h>
#include "/obj/daemon/chatd.h"

#define USAGE "subscribe <channel>\n"

int
main(string str)
{
  chan_i info;

  if( !str ) {
    write("Subscribed to:\n");
    write(wrap(implode((string *)this_player()->query_channels(), ", ")+"\n"));
    return 1;
  }

  if( !(info = CHAT_D->query_channel_info(str)) )
    return notify_fail(sprintf("Channel '%s' does not exist.\n", str));

  // level & guild checks here

  if( this_player()->is_subscribed_chan(str) )
    return notify_fail("You are already subscribed!\n");

  this_player()->set_channels( this_player()->query_channels() + ({ str }) );

  CHAT_D->add_member(str, this_player());

  write("Subscribed.\n");

  return 1;
}
