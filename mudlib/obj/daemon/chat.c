/*    /daemon/chat.c
 *    from the Nightmare 3.2 Mudlib
 *    daemon to handle all mud chat channels
 *    created by Descartes of Borg 931220
 */

#include <mudlib.h>
#include <net/daemons.h>
#include <origin.h>
#include "chat.h"

inherit DAEMON;

static private mapping Channels;

static void create() {
    object pl;

    daemon::create();
    SetNoClean(1);
    Channels = ([]);
//    foreach(pl in users()) {
//	string channel;

//	foreach(channel in (string *)pl->GetChannels()) {
//            if( !Channels[channel] ) Channels[channel] = ({});
//            Channels[channel] = distinct_array(Channels[channel] + ({ pl }));
//        }
//    }
}

string *eventRegisterMember(string *chans) {
    string *tmp;
    object ob;
    string channel;

    if( !living(ob = previous_object()) ) return ({});
    tmp = ({});
    foreach(channel in chans) {
	/* just check out for secure channels */
	switch(channel) {
            case "admin":
// temp	      if( !archp(ob) ) break;
            case "cre": case "intercre": case "gossip": case "intergossip":
// temp	      if( !creatorp(ob) ) break;
            default:
	      if( !Channels[channel]) Channels[channel] = ({});
	      Channels[channel] = distinct_array(Channels[channel] + ({ ob }));
	      tmp += ({ channel });
	}
    }
    return tmp;
}

string *eventRemoveMember(string *chans) {
    object ob;
    string channel;

    if( !living(ob = previous_object()) ) return({});
    foreach(channel in chans) {
        if( !Channels[channel] ) continue;
        else Channels[channel] -= ({ ob });
        if( !sizeof(Channels[channel]) ) map_delete(Channels, channel);
    }
    return chans;
}

int cmdChannel(string verb, string str) {
    string msg, name, rc;
    int emote;

    if( verb == "list" ) {
	string *who;
	string ch, mud;

	if( !str ) return 0;
	if( sscanf(str, "%s@%s", ch, mud) == 2 ) {
	    if( !Channels[ch] ) return 0;
	    if( member_array(this_player(), Channels[ch]) == -1 ) return 0;
	    if( ch == (ch = GetRemoteChannel(ch)) ) return 0;
	    if( !(mud = (string)INTERMUD_D->GetMudName(mud)) ) {
		message("system", "No such MUD is known.\n", this_player());
		return 1;
	    }
	    SERVICES_D->eventSendChannelWhoRequest(ch, mud);
	    message(ch, "Remote listing request sent.\n", this_player());
	    return 1;
	}
	else ch = str;
	if( !Channels[ch] ) return 0;
	if( member_array(this_player(), Channels[str]) == -1 ) return 0;
	who = GetChannelList(str);
	msg = "Online: " + implode(who, "   ") +"\n";
	message(str, msg, this_player());
	return 1;
    }
    if( !Channels[verb] ) {
        if( sscanf(verb, "%semote", verb) ) {
	    emote = 1;
	    if( strsrch(str, "$N") == -1 ) str = "$N " + str;
	}
        else return 0;
        if( !Channels[verb] ) return 0;
    }
    rc = GetRemoteChannel(verb);
    if( member_array(this_player(), Channels[verb]) == -1 ) return 0;
//    if( !str || str == "" ) {
//        this_player()->SetBlocked(verb);
//        return 1;
//    }
    if( (int)this_player()->GetBlocked(verb) ) {
        if( (int)this_player()->GetBlocked("all") ) {
            message(verb, "You cannot chat while totally blocked.\n", 
		    this_player());
            return 1;
        }
        this_player()->SetBlocked(verb);
    }
//    if( verb == "admin" || verb=="cre" ) {
//        if( !(name = (string)this_player()->GetCapName()) )
//          name = capitalize((string)this_player()->GetKeyName());
//    }
    else name = (string)this_player()->query_cap_name();
    eventSendChannel(name, verb, str, emote, 0, 0);
    if( rc != verb ) 
      SERVICES_D->eventSendChannel(name, rc, str, emote, 0, 0);
    return 1;
}

varargs void eventSendChannel(string who, string ch, string msg, int emote,
			      string target, string targmsg) {
    if( file_name(previous_object()) == SERVICES_D ) ch = GetLocalChannel(ch);
    else if( origin() != ORIGIN_LOCAL ) return;
    if( emote ) {
	object ob;
	string this_msg;

	if( target ) {
	    ob = find_player(target);
	    target = (string)ob->query_name();
	}
	this_msg = "%^MAGENTA%^<" + ch + ">%^RESET%^ ";
	msg = replace_string(msg, "$N", who);
	if( target ) {
	    msg = replace_string(msg, "$O", target);
	    targmsg = replace_string(targmsg, "$N", who);
	    targmsg = capitalize(replace_string(targmsg, "$O", "you"));
	}
	message(ch, this_msg + msg+"\n", Channels[ch], (ob ? ({ ob }) : ({})));
	if( ob ) message(ch, this_msg + targmsg +"\n", ob);
    }
    else {
	msg = who + " %^MAGENTA%^<" + ch + ">%^RESET%^: " + msg +"\n";
	message(ch, msg, Channels[ch]);
    }
}

string *GetChannelList(string ch) {
    string list;
    int i;

    if( file_name(previous_object()) == SERVICES_D ) ch = GetLocalChannel(ch);
    else if( origin() != ORIGIN_LOCAL ) return ({});
    if( !Channels[ch] ) return ({});
    return map(filter(Channels[ch], (: $1 && !((int)$1->GetInvis()) :)), 
		      (: (string)$1->query_cap_name() :));
}

string GetLocalChannel(string ch) {
    switch(ch) {
	case "imud_code":
	return "intercre";

	case "imud_gossip":
	return "intergossip";

        case "ie_flibcode":
        return "foundation";
    }
    return ch;
}

string GetRemoteChannel(string ch) {
    switch(ch) {
	case "intercre": 
	return "imud_code";

	case "intergossip":
	return "imud_gossip";
        case "foundation":
        return "ie_flibcode";
    }
    return ch;
}
