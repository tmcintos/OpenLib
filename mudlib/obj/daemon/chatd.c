/*  -*- LPC -*-  */
#include <mudlib.h>
#include <daemons.h>
#include <net/daemons.h>
#include <save.h>
#include <origin.h>
#include "chatd.h"

#define DEBUG

inherit DAEMON;

private mapping channels;

void
create()
{
  daemon::create();

  SetNoClean(1);
  channels = ([]);
  restore_object(SAVE_CHATD, 1);

  foreach(string chan in keys(channels)) {
    ((chan_i) channels[chan])->userlist = ({});
    
    foreach(object user in users()) {
      if(user->is_subscribed_chan(chan))
	((chan_i) channels[chan])->userlist += ({ user });
    }
  }
  // Assert: all channels' userlists are up-to-date
}

#ifdef DEBUG

mapping
dump_chanlist()
{
  return channels;
}

#endif /* DEBUG */

int
add_channel(string name, chan_i info)
{
  if(!archp(this_interactive())) return 0;
  if(channels[name]) return 0;

  channels += ([ name : info ]);

  save_object(SAVE_CHATD);
  return 1;
}

int
remove_channel(string name)
{
  if(!archp(this_interactive())) return 0;
  if(!channels[name]) return 0;

  map_delete(channels, name);

  save_object(SAVE_CHATD);
  return 1;
}

chan_i
query_channel_info(string name)
{
  if(channels[name])
    return copy((chan_i) channels[name]);
  else
    return 0;
}

void
add_member(string chan, object member)
// It's the subscribe cmd's place to figure out restrictions...not this
{
  ((chan_i) channels[chan])->userlist += ({ member });
}

void
remove_member(string chan, object member)
{
  ((chan_i) channels[chan])->userlist -= ({ member });
}

string *
query_channel_members(string chan)
{
  chan = map_to_local(chan);

  if(!channels[chan]) return 0;

  return (string *)
    map((object *) ((chan_i) channels[chan])->userlist,
	function(object ob) {
	  return (string) ob->query_cap_name();
	}
	);
}

void
show_channels()
{
  string ch;
  chan_i info;

  printf("%-10s  %-45s  %-10s  %7s\n",
	 "Channel", "Description", "Guild", "Min Lvl");
  printf("%-10s  %-45s  %-10s  %5s\n",
	 "-------", "-----------", "-----", "-----");

  foreach(ch, info in channels) {
    printf("%-10.10s  %-45.45s  %-10.10s  %-3.3s\n",
	   ch, info->desc,
	   (info->guild ? (string) info->guild : "none"),
	   (info->min_lvl != -1 ? (string) info->min_lvl : "n/a"));
  }
}

varargs
void
broadcast_local(string ch, string who, string msg,
	  int emote, string target, string targetmsg)
{
  object user;
  string mess;

  if( file_name(previous_object()) == SERVICES_D )
    ch = map_to_local(ch);
  else if( origin() != ORIGIN_LOCAL )
    return;

  if(!channels[ch])
    return;

  mess = "%^L_BLUE%^[" + ch + "]%^RESET%^: ";
  msg = replace_string(msg, "$N", who);

  if(emote) {
    if(target) {
      object targetob = find_player(target);

      if(!targetob)
	return;
      
      targetmsg = replace_string(targetmsg, "$N", who);
      targetmsg = replace_string(targetmsg, "$O", target);
      msg = replace_string(msg, "$O", target);

      foreach(user in ((chan_i) channels[ch])->userlist) {
	if(user == targetob) {
	  mess += targetmsg + "\n";
	} else {
	  mess += msg + "\n";
	}
	tell_object(user, mess);
      }
      return;
    } else {
      mess += msg + "\n";

      foreach(user in ((chan_i) channels[ch])->userlist)
	tell_object(user, mess);

      return;
    }
  } else /* if(!emote) */ {
    mess = sprintf("%s %s%s\n", who, mess, msg);

    foreach(user in ((chan_i) channels[ch])->userlist)
      tell_object(user, mess);

    return;
  }
}

int
chat(string str)
{
  string who = this_player()->query_cap_name();
  string chan, mess, type, target, targetmsg;
  string rchan;
  int emote;

  if( sscanf(str, "%s %s: %s", type, chan, mess) == 3 ) {
    switch(type) {
    case "emote":
      emote = 1;
      mess = sprintf("%s %s", who, mess);
      break;
    case "feeling":
      emote = 0;
//      mess = FEELING_D->get_feeling(mess);
      break;
    default:
    }
  } else if( sscanf(str, "%s: %s", chan, mess) != 2 ) {
    return 0;    
  }

  if(chan == "list") {
    string ch, mud;
    string* tmp;

    if( sscanf(mess, "%s@%s", ch, mud) != 2 )
      ch = mess;
      
    tmp = query_channel_members(ch);

    if(!arrayp(tmp)) {
      write("No such channel.\n");
      return 1;
    } else {
      if(!mud) {
	write(wrap("Users: " + implode(tmp, " ") + "\n"));
      } else {
	
	if( ch == (ch = map_to_remote(ch)) ) {
	  write("Not a remote channel.\n");
	  return 1;
	}
	if( !(mud = INTERMUD_D->GetMudName(mud)) ) {
	  write("Can't find that MUD.\n");
	  return 1;
	}
	SERVICES_D->eventSendChannelWhoRequest(ch, mud);
	write("Remote channel listing request sent.\n");
      }
    }

    return 1;
  }

  if( !(int) this_player()->is_subscribed_chan(chan) )
    return notify_fail("You can't chat on a channel to which "
		       "you aren't subscribed!\n");

  if( !channels[chan] )
    return notify_fail("Channel does not exist.\n");
  if( ((chan_i) channels[chan])->min_lvl == -1 )
    return notify_fail("Channel is info only.\n");

  rchan = map_to_remote(chan);

  broadcast_local(chan, who, mess, emote, target, targetmsg);

  if(rchan != chan)
    SERVICES_D->eventSendChannel(who, rchan, mess, emote, target, targetmsg);

  return 1;
}

string
map_to_local(string ch)
{
  switch(ch) {
  case "imud_code":
    return "icode";
  case "imud_gossip":
    return "igossip";
  }
  log_file("chatd.tmp", "maptolocal: "+ch+"\n");

  return ch;
}

string
map_to_remote(string ch)
{
  switch(ch) {
  case "icode":
    return "imud_code";
  case "igossip":
    return "imud_gossip";
  }
  log_file("chatd.tmp", "maptoremote: "+ch+"\n");

  return ch;
}
