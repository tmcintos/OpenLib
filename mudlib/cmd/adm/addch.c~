/*  -*- LPC -*-  */
#include <command.h>
#include <daemons.h>
#include "/obj/daemon/chatd.h"

#define USAGE "usage: addch <channelname> (<descrip>) [guild] <min level>\n"

int
main(string str)
{
  chan_i newchan;
  string name, desc, guild;
  int lev;

  if(!str)
    return notify_fail(USAGE);

  if(sscanf(str, "%s (%s) %s %d", name, desc, guild, lev) != 4 &&
     sscanf(str, "%s (%s) %d", name, desc, lev) != 3)
    return notify_fail(USAGE);

  newchan = new(chan_i);

  newchan->desc = desc;
  newchan->min_lvl = lev;
  newchan->guild = guild;
  newchan->userlist = ({});

  if(CHAT_D->add_channel(name, newchan)) {
    printf("Channel %s added\n", name);
    return 1;
  } else {
    write("Failed.\n");
    return 1;
  }
}
