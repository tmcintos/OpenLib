/*  -*- LPC -*-  */
// inetd.c:  OOB superserver for I3
//
// created 12.8.95 by Tim@Dysfunctional Mud

#include <mudlib.h>
#include <net/daemons.h>
#include <net/network.h>
#include "inetd.h"

// does what you think it does
#define DEBUG "inetd"

inherit DAEMON;
inherit "/obj/daemon/net/client";
inherit "/obj/daemon/net/server";

private static mapping connections;        // connections
private mapping queue;              // outgoing packet queue

private
void
create()
{
  daemon::create();
  server::create();

  client::SetRead((: eventClientRead :));
  connections = ([]);
  queue = ([]);
  restore_object("/tmp/inetd", 1);
  call_out( (: setup :), 2);
  call_out((:eventAdvanceQueue:), 1200);
}

void
remove()
{
  save_object("/tmp/inetd");
}

private
void
setup()
{
  if( server::eventCreateSocket(PORT_OOB, MUD) < 0 ) {
    if(this_object())
      destruct(this_object());
    return;
  }
}

private
void
eventSocketClosed(int fd)
{
  map_delete(connections, fd);
}

private
void
eventWrite(int fd, mixed *packet)
{
  switch(fd) {
    case -2:
      client::eventWrite(packet);
      break;
    default:
      server::eventWrite(fd, packet);
  }
}

private
void
eventClientRead(mixed *packet)
{
  eventRead(-2, packet);
}

private
void
eventRead(int fd, mixed *packet)
{
  if(!packet || !sizeof(packet)) return; /* error here when implemented */

//  Check if we're talking to a new client
  if(!connections[fd]) {
    if(packet[0] != "oob-begin") {
      return CloseSocket(fd);
    } else {
      connections[fd] = new(conn);
    }
  }

#ifdef DEBUG
  log_file(DEBUG, sprintf("rcv: fd[%d] pkt[0]=%s\n", fd, packet[0]));
#endif

  switch(packet[0]) {
    case "oob-begin":
      if(fd == -2) {
	eventSendPackets(fd, ((conn) connections[fd])->mudname);
	break;
      }

      ((conn) connections[fd])->mudname = packet[1];

      // do authentication if wanted
      if(!packet[2]) {
	((conn) connections[fd])->auth = 0;
      } else {
	if(!AUTH_D->authenticate_mud(packet[1], packet[3]))
	  CloseSocket(fd);
	else
	  ((conn) connections[fd])->auth = 1;
      }
      eventWrite(fd, ({ "oob-begin", mud_name(), 0, 0 }));  // ready for xfer
      break;
    case "oob-end":
      eventSendPackets(fd, packet[1]);
      break;
    case "mail": // sends mail-ack back
      if(!((conn) connections[fd])->auth) {
	// ERROR not authenticated
      }
      SERVICES_D->eventRecieveMail(packet);
      break;
    case "mail-ack":
      SERVICES_D->eventRecieveMailAck(packet);
      break;
    case "file-list-req": // sends file-list-req back
      if(!((conn) connections[fd])->auth) {
	// ERROR not authenticated
      }
      SERVICES_D->eventRecieveFileListRequest(packet);
      break;
    case "file-list-reply":
      SERVICES_D->eventRecieveFileListReply(packet);
      break;
    case "file-get-req": // sends file-get-reply back
      if(!((conn) connections[fd])->auth) {
	// ERROR not authenticated
      }
      SERVICES_D->eventRecieveFileGetRequest(packet);
      break;
    case "file-get-reply":
      SERVICES_D->eventRecieveFileGetReply(packet);
      break;
    case "file-put": // sends file-put-ack back
      if(!((conn) connections[fd])->auth) {
	// ERROR not authenticated
      }
      SERVICES_D->eventRecieveFilePut(packet);
      break;
    case "file-put-ack":
      SERVICES_D->eventRecieveFilePutAck(packet);
      break;
  }
}

private
void
eventSendPackets(int fd, string target_mudname)
{
  if(!queue[target_mudname] || !sizeof(queue[target_mudname])) {
    if(fd == -2) {
      client::CloseSocket();
    } else {
      eventWrite(fd, ({ "oob-end", mud_name() }));
    }
  } else {
    eventWrite(fd, queue[target_mudname][0]);
    queue[target_mudname] = queue[target_mudname][1..];

    call_out((:eventSendPackets:), 1, fd, target_mudname);
  }
}

void
AddPacketToQueue(string mudname, mixed *packet)
{
  if(!queue[mudname]) queue[mudname] = ({});

  queue[mudname] += ({ packet });
}

#ifdef DEBUG
mapping
dump_queue()
{
  return queue;
}
#endif

// this is all messed up..need to slow things down & do it in order

void
eventAdvanceQueue()
{
  mapping mudlist = INTERMUD_D->GetMudList();
  string *muds = keys(queue), mud;
  string addr, nm; int port;
  int i;

  for(i=0; i < sizeof(muds); i++) {
    if(sizeof(queue[muds[i]])) break;
  }
  mud = muds[i];

#ifdef DEBUG
  log_file(DEBUG, sprintf("advancing queue: mud=%s\n", mud));
#endif

  nm = INTERMUD_D->GetMudName(mud);
  
  if(nm) {
    addr = mudlist[nm][1];
    port = mudlist[nm][3];
      
    if(client::eventCreateSocket(addr, port) < 0) return;

    connections[-2] = new(conn);
    ((conn) connections[-2])->mudname = nm;
    client::eventWrite(({ "oob-begin", mud_name(), 1,
			  AUTH_D->get_session_key(nm) }));
  } else {
    map_delete(queue, nm);
  }
  call_out((:eventAdvanceQueue:), 1200);
}
