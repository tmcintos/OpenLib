/*  -*- LPC -*-  */
// chatd.c
// Originally written by Tim in fall 1995.
// 03/08/96  Tim  :fixed emote to work with I3; documented code somewhat;
//                 took out query_channel_info() and replaced with
//                 is_qualified() (empty right now) for checking eligibility
#include <mudlib.h>
#include <daemons.h>
#include <net/daemons.h>
#include <save.h>
#include <origin.h>
#include "chat_d.h"

#undef DEBUG

inherit DAEMON;

private mapping channels;
private string* chanmap;   // even idx is local name; odd idx is remote name

void
create()
{
  daemon::create();

  channels = ([]);
  chanmap = ({});
  restore_object(SAVE_CHAT_D, 1);

  foreach(string chan in keys(channels)) {
    ((class channel_info) channels[chan])->userlist = ({});
    
    foreach(object user in users()) {
      if(user->is_subscribed_chan(chan))
	add_member(chan, user);
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

// pre: true
// post: returns 1 if channel exists
int
channel_exists(string name)
{
  if(channels[name])
    return 1;
  else
    return 0;
}

// pre: true
// modifies: self, savefile
// post: there is now a local<->remote name mapping for channel
void
set_alias(string localname, string remotename)
{
  if(!archp(this_interactive())) return 0;
  // more validity checks to do here

  chanmap += ({ localname, remotename });
  save_object(SAVE_CHAT_D);
}

// pre: true
// modifies: self, savefile
// post: returns 1 if add was successful, 0 if channel exists or fail
int
add_channel(string name, string desc, int level, string guild)
{
  class channel_info newchan;

  if(!archp(this_interactive())) return 0;
  if(channel_exists(name)) return 0;

  newchan = new(class channel_info);
  newchan->desc = desc;
  newchan->min_lvl = level;
  newchan->guild = guild;
  newchan->userlist = ({});

  channels += ([ name : newchan ]);

  save_object(SAVE_CHAT_D);
  return 1;
}

// pre: true
// modifies: self, savefile
// post: returns 1 if channel 'name' was successfully deleted
//       returns 0 for fail or no-such-channel
int
remove_channel(string name)
{
  if(!archp(this_interactive())) return 0;
  if(!channel_exists(name)) return 0;

  map_delete(channels, name);

  save_object(SAVE_CHAT_D);
  return 1;
}

// pre: ob == a user
// post: returns 1 if ob is qualified to join channel chan
int
is_qualified(object ob, string chan)
{
  return 1;
}

// pre: member is qualified to join channel chan
// modifies: self
// post: member is now a member of channel chan
void
add_member(string chan, object member)
{
  if( member_array(member,
		   ((class channel_info) channels[chan])->userlist) != -1 )
    return;

  ((class channel_info) channels[chan])->userlist += ({ member });
}

// pre: member is a member of channel chan
// modifies: self
// post: member is no longer a member of channel chan
void
remove_member(string chan, object member)
{
  ((class channel_info) channels[chan])->userlist -= ({ member });
}

// pre: true
// modifies: none
// post: returns array of members of channel 'chan'
string *
query_channel_members(string chan)
{
  chan = map_to_local(chan);

  if(!channels[chan]) return 0;

  return (string *)
    map((object *) ((class channel_info) channels[chan])->userlist,
	(: $1->query_cap_name() :));
}

// pre: this_player() != 0
// modifies: none
// post: list of channels is displayed to this_player()
void
show_channels()
{
  write("[* means channel is an Intermud channel]\n");
  printf("%-10s  %-45s  %-10s  %7s\n",
	 "Channel", "Description", "Guild", "Min Lvl");
  printf("%-10s  %-45s  %-10s  %5s\n",
	 "-------", "-----------", "-----", "-----");

  foreach(string ch in sort_array(keys(channels), (: strcmp :))) {
    class channel_info info = channels[ch];

    printf("%:-10s %c%:-45s  %:-10s  %:-3s\n",
	   ch, (ch == map_to_remote(ch) ? ' ' : '*'), info->desc,
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

      foreach(user in ((class channel_info) channels[ch])->userlist) {
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

      foreach(user in ((class channel_info) channels[ch])->userlist)
	tell_object(user, mess);

      return;
    }
  } else /* if(!emote) */ {
    mess = sprintf("%s %s%s\n", who, mess, msg);

    foreach(user in ((class channel_info) channels[ch])->userlist)
      tell_object(user, mess);

    return;
  }
}

// chat command
// pre: this_player() != 0
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
      mess = sprintf("$N %s", mess);
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

  if( !channel_exists(chan) )
    return notify_fail("Channel does not exist.\n");
  if( ((class channel_info) channels[chan])->min_lvl == -1 )
    return notify_fail("Channel is info only.\n");

  rchan = map_to_remote(chan);

  broadcast_local(chan, who, mess, emote, target, targetmsg);

  if(rchan != chan)
    SERVICES_D->eventSendChannel(who, rchan, mess, emote, target, targetmsg);

  return 1;
}

// pre: true
// modifies: none
// post: returns local name of remote-channel 'ch'
string
map_to_local(string ch)
{
  int i = member_array(ch, chanmap);

  if(i > 0 && i % 2) { /* Odd */
    ch = chanmap[i-1];
  }

  return ch;
}

// pre: true
// modifies: none
// post: returns remote name of local-channel 'ch'
string
map_to_remote(string ch)
{
  int i = member_array(ch, chanmap);

  if(i >= 0 && !(i % 2)) { /* Even */
    ch = chanmap[i+1];
  }

  return ch;
}
