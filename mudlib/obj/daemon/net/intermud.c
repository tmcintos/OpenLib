/*    /daemon/intermud.c
 *    from the Nightmare IV LPC Library
 *    daemon handling the InterMUD-3 Protocol
 *    created by Descartes of Borg 950506
 */

#include <mudlib.h>

#include <save.h>
#include <daemons.h>
#include <net/network.h>
#include <net/config.h>
#include <net/daemons.h>
#include "intermud.h"

inherit TCP_CLIENT;

private class list MudList, ChannelList;
private mapping Banned;
private mixed *Nameservers;
private static int Connected;

static void create() {
    client::create();
    Connected = 0;
    Banned = ([]);
    Nameservers = ({ ({ "*gjs", "199.199.122.10 9000" }) });
    MudList = new(class list);
    ChannelList = new(class list);
    MudList->ID = -1;
    MudList->List = ([]);
    ChannelList->ID = -1;
    ChannelList->List = ([]);
    if( file_size( SAVE_INTERMUD __SAVE_EXTENSION__ ) > 0 )
//      unguarded( (: restore_object, SAVE_INTERMUD, 1 :) );
      restore_object(SAVE_INTERMUD, 1);
    SetNoClean(1);
    SetDestructOnClose(1);
    SetSocketType(MUD);
    call_out( (: Setup :), 1, Nameservers[0][0], 0 );
}

static void Setup(string ns, int x) {
    string ip;
    int port;

    if( x >= sizeof(Nameservers) || x < 0 ) 
      error("Failed to locate an active nameserver.\n");
    if( Nameservers[x][0] != ns ) return;
    if( x + 1 < sizeof(Nameservers) )
      call_out( (: Setup :), 30, Nameservers[x+1][0], x+1);
    else call_out((: eventConnectionFailure :), 30);
    sscanf(Nameservers[x][1], "%s %d", ip, port);
    if( eventCreateSocket(ip, port) < 0 ) return;
    eventWrite( ({ "startup-req-1", 5, mud_name(), 0, ns, 0, "password",
		  MudList->ID, ChannelList->ID, query_host_port(), PORT_OOB, 
		  PORT_UDP, mudlib() + " " + mudlib_version(), 
		  mudlib() + " " + mudlib_version(), version(), "LPMud",
		  "alpha development", INTERMUD_SERVICES }) );
}

static void eventRead(mixed *packet) {
    string *cles;
    mixed val;
    string ns, cle;
    int i, maxi;

    if( !packet || sizeof(packet) < 6 ) return; /* should send error */
    if( Banned[packet[1]] ) {
	/* build error packet, let them know why they are banned */
	return;
    }
    switch(packet[0]) {
	case "startup-reply":
	  if( sizeof(packet) < 7 ) return;  /* should send error */
	  if( !sizeof(packet[6]) ) return;
	  if( packet[6][0][0] == Nameservers[0][0] ) {
	      Nameservers = packet[6];
	      Connected = Nameservers[0][0];
	      eventRequestMudList();
	      eventRequestChannelList();
	  }
	  else {
	      Nameservers = packet[6];
	      Setup(Nameservers[0][0], 0);
	  }
	  return;
        case "mudlist":
//	  if( packet[6] == MudList->ID ) return; 
	  if( packet[2] != Nameservers[0][0] ) return;
	  MudList->ID = packet[6];
	  foreach(cle, val in packet[7]) {
	      if( !val && MudList->List[cle] != 0 ) 
		map_delete(MudList->List, cle);
	      else if( val ) MudList->List[cle] = val;
	  }
//	  MudList->List = packet[7];
	  return;
	case "channel-e":
	  SERVICES_D->eventReceiveChannelEmote(packet);
	  break;
	case "channel-m":
	  SERVICES_D->eventReceiveChannelMessage(packet);
	  break;
	case "chan-who-reply":
	  SERVICES_D->eventReceiveChannelWhoReply(packet);
	  break;
	case "chan-who-req":
	  SERVICES_D->eventReceiveChannelWhoRequest(packet);
	  break;
        case "chan-user-req":
	  SERVICES_D->eventReceiveChannelUserRequest(packet);
	  break;
        case "chanlist-reply":
//	  if( packet[6] == ChannelList->ID ) return; 
	  if( packet[2] != Nameservers[0][0] ) return;
	  ChannelList->ID = packet[6];
	  foreach(cle, val in packet[7]) { 
	      if( !val && ChannelList->List != 0 ) 
		map_delete(ChannelList->List, cle);
	      else if( val ) ChannelList->List[cle] = val;
	  } 
	  SERVICES_D->eventRegisterChannels(packet[7]);
	  return;
	case "finger-req":
	  SERVICES_D->eventReceiveFingerRequest(packet);
	  break;
	case "finger-reply":
	  SERVICES_D->eventReceiveFingerReply(packet);
	  break;
        case "locate-req":
	  SERVICES_D->eventReceiveLocateRequest(packet);
	  break;
        case "locate-reply":
	  SERVICES_D->eventReceiveLocateReply(packet);
	  break;
        case "tell":
	  SERVICES_D->eventReceiveTell(packet);
	  break;
        case "who-req":
	  SERVICES_D->eventReceiveWhoRequest(packet);
	  break;
        case "who-reply":
	  SERVICES_D->eventReceiveWhoReply(packet);
	  break;
        case "news":
	  SERVICES_D->eventReceiveNews(packet);
	  break;
        case "mail":
	  SERVICES_D->eventReceiveMail(packet);
	  break;
        case "mail-ok":
	  SERVICES_D->eventReceiveMailOk(packet);
	  break;
        case "file":
	  break;
        case "error":
	  SERVICES_D->eventReceiveError(packet);
	  break;
        default:
	  break;
    }
}

static void eventRequestMudList() {
    eventWrite( ({ "mudlist-request", 5, mud_name(), 0, 
		   Nameservers[0][0], 0, MudList->ID }) );
}

static void eventRequestChannelList() {
    eventWrite( ({ "chanlist-req", 5, mud_name(), 0, Nameservers[0][0], 0,
		   ChannelList->ID }) );
}

static void eventSocketClose() {
    Connected = 0;
    Setup(Nameservers[0][0], 0);
}

static void eventConnectionFailure() {
    if( Connected ) return;
    error("Failed to find a useful name server.\n");
}

int SetDestructOnClose(int x) { return 0; }

int SetSocketType(int x) { return client::SetSocketType(MUD); }

string GetMudName(string mud) {
    string *lc, *uc;
    int x;

    if( MudList->List[mud] ) return mud;
    lc = map(uc = keys(MudList->List), (: lower_case :));
    x = member_array(lower_case(mud), lc);
    if( x < 0 ) return 0;
    else return uc[x];
}

mapping GetMudList() { return copy(MudList->List); }

string *GetMuds() { return keys(MudList->List); }

mapping GetChannelList() { return copy(ChannelList->List); }

string *GetChannels() { return keys(ChannelList->List); }

string *GetMatch(string mud) {
    string *uc, *lc;

    mud = lower_case(mud);
    lc = map(uc = keys(MudList->List), (: lower_case :));
    return map(filter(regexp(lc, "^"+mud, 1), (: intp :)), (: $(uc)[$1] :));
}

string GetNameserver() { return Nameservers[0][0]; }
