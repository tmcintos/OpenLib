/*  -*- LPC -*-  */
// inetd.c:  OOB superserver for I3
//
// created 12.8.95 by Tim@Dysfunctional Mud
//
// This object maintains a queue of outgoing packets and also handles
// incoming connections for OOB services.

#include <mudlib.h>
#include <net/config.h>
#include <net/daemons.h>
#include <net/network.h>
#include "inetd.h"

inherit TCP_SERVER;

static object client;              // Client object
static int client_busy;            // 1 if client is busy with a connection
static mapping connections;        // connections to server

element* packet_q;                 // outgoing packet queue (saved)

#define CONNECTION(x) ((class _conn) connections[x])

void
create()
{
  ::create();

  restore_object(SAVE_INET_D);

  connections = ([]);
  packet_q = ({});

  client = new(TCP_CLIENT);
  client->SetSocketType(MUD);
  client->SetRead((: eventRead, -2 :));
  client->SetSocketClose((: eventSocketClosed, -2 :));

  /*
   * start outgoing queue
   */
  call_out("eventAdvanceQueue", TIME_TO_ADVANCE);
  call_out("setup", 2);
}

int
remove()
{
  if(!destruct(client))
    log_file(INET_D_LOG, "remove: couldn't destruct client object.\n");

  remove_call_out("setup");
  remove_call_out("eventAdvanceQueue");
  remove_call_out("eventSendPackets");
  save_object(SAVE_INET_D);
}

void
setup()
{
  if( eventCreateSocket(PORT_OOB, MUD) < 0 ) {
    log_file(INET_D_LOG, "Couldn't create socket for server.\n");

    if(this_object())
      destruct(this_object());
  }
}

static
void
eventSocketClosed(int fd)
{
#ifdef DEBUG
  DEBUG( sprintf("Socket fd=(%d) closed.\n", fd) );
#endif

  if(fd == -2)
    client_busy = 0;
  map_delete(connections, fd);
}

void
eventWrite(int fd, mixed* packet)
{
#ifdef DEBUG
  DEBUG( sprintf("evWrite(%d, %O)\n", fd, packet) );
#endif

  if(fd == -2)
    client->eventWrite(packet);
  else
    ::eventWrite(fd, packet);
}

void
eventRead(int fd, mixed val)
{
  mixed* packet = (mixed*) val;

  if(!packet || !sizeof(packet)) {
    // ERROR when implemented
    return;
  }

#ifdef DEBUG
  DEBUG( sprintf("evRead(%d, %O)\n", fd, packet) );
#endif

  /*
   * Check if we're talking to a new client.  If we are, and the
   * 1st packet they're sending us isn't an oob-begin, we ignore it.
   */
  if(!CONNECTION(fd)) {
    if(packet[0] != "oob-begin") {
      // ERROR when implemented
      return;
    } else {
      CONNECTION(fd) = new(conn);
    }
  }

  /*
   * Here are the guts of the whole protocol.  Depending on what packet
   * we get, we call a function in SERVICES_D to handle it.
   */
  switch(packet[0]) {
    case "oob-begin":
      /*
       * If we're getting this from the client it means we're ready to go
       */
      if(fd == -2) {
	eventSendPackets(fd, CONNECTION(fd)->mudname);
	break;
      }

      /*
       * New connection
       * Do authentication if wanted
       */
      CONNECTION(fd)->mudname = packet[1];

      if(!packet[2]) {
	CONNECTION(fd)->auth = 0;
      } else {
	/*
	 * If auth fails we shut it down now
	 */
	if(!AUTH_D->authenticate_mud(packet[1], packet[3]))
	  socket_close(fd);
	else
	  CONNECTION(fd)->auth = 1;
      }
      /*
       * Ready for transfer
       */
      eventWrite(fd, ({ "oob-begin", mud_name(), 0, 0 }));
      break;
    case "oob-end":
      /*
       * Now we can send more packets if we want to
       */
      eventSendPackets(fd, packet[1]);
      break;
    case "mail":                        /* sends mail-ack back */
      if(!CONNECTION(fd)->auth) {
	// ERROR not authenticated
	break;
      }
      SERVICES_D->eventReceiveMail(packet);
      break;
    case "mail-ack":                    /* take note they acknowledged */
      SERVICES_D->eventReceiveMailAck(packet);
      break;
    case "file-list-req":               /* sends file-list-req back */
      if(!CONNECTION(fd)->auth) {
	// ERROR not authenticated
	break;
      }
      SERVICES_D->eventReceiveFileListRequest(packet);
      break;
    case "file-list-reply":             /* decode file-list-reply packet */
      SERVICES_D->eventReceiveFileListReply(packet);
      break;
    case "file-get-req":                /* sends file-get-reply back */
      if(!CONNECTION(fd)->auth) {
	// ERROR not authenticated
	break;
      }
      SERVICES_D->eventReceiveFileGetRequest(packet);
      break;
    case "file-get-reply":              /* decode file-get-reply packet */
      SERVICES_D->eventReceiveFileGetReply(packet);
      break;
    case "file-put":                    /* sends file-put-ack back */
      if(!CONNECTION(fd)->auth) {
	// ERROR not authenticated
	break;
      }
      SERVICES_D->eventReceiveFilePut(packet);
      break;
    case "file-put-ack":                /* take note they acknowledged */
      SERVICES_D->eventReceiveFilePutAck(packet);
      break;
    case "error":                       /* decode error packet */
      SERVICES_D->eventReceiveErrorOOB(packet);
      break;
    default:
  }
}

void
eventSendPackets(int fd, string target_mudname)
{
  element entry = GetMudFromQueue(target_mudname);

#ifdef DEBUG
  DEBUG( sprintf("evSndPkts(%d, %s)\n", fd, target_mudname) );
#endif
  
  if(!entry || !sizeof( entry->val )) {
    /*
     * We're done; close up shop
     * If we're the client we just dump the connection; otherwise send
     * an oob-end in case the other guy has some more stuff for us.
     */
    if(fd == -2) {
      client->CloseSocket();
    } else {
      eventWrite(fd, ({ "oob-end", mud_name() }));
    }
  } else {
    /*
     * Take 1 packet from this mud's queue and send it out
     */
    eventWrite( fd, (mixed*) entry->val[0] );
    entry->val = entry->val[1..];

    call_out("eventSendPackets", 1, fd, target_mudname);
  }
}

// POST: returns element with key 'mudname' if it is in the queue,
//       otherwise returns 0
element
GetMudFromQueue(string mudname)
{
  int i = sizeof(packet_q) - 1;

  while(i >= 0) {
    if(packet_q[i]->key == mudname) break;       // check for existing queue
    i--;
  }

  if(i < 0)
    return 0;
  else
    return packet_q[i];
}

// PRE: mudname is an existing mud in the form returned by
//      INTERMUD_D->GetMudName(name)
void
AddPacketToQueue(string mudname, mixed* packet)
{
  element elt = GetMudFromQueue(mudname);

  if(!elt) {
    elt = new(element);                 // setup new queue
    elt->key = mudname;
    elt->val = ({ packet });
    packet_q += ({ elt });
  } else {
    elt->val += ({ packet });           // add to existing queue
  }
}

boolean
IsConnected(string mud)
{
  foreach(int key, conn val in connections)
    if(val->mudname == mud) return 1;

  return 0;
}
    
// Gets the first existing mud with packets waiting from the queue

mixed*
GetNextMud()
{
  mapping mudlist = INTERMUD_D->GetMudList();
  int i = sizeof(packet_q) - 1;
  string mud;

  while(i >= 0) {
    if(sizeof( packet_q[i]->val ) && !IsConnected( packet_q[i]->key ))
      break;
    i--;
  }
  /* Returns 0 if queue is empty
   * Service the 1st unconnected mud with packets in its queue
   * Returns 0 if no mud needing service
   */
  if(i >= 0) {
    mud = INTERMUD_D->GetMudName( packet_q[i]->key );
    
    if(mud) {
      return ({ mud }) + mudlist[mud];
    } else {
      /*
       * Mud no longer exists! Delete and get another.
       */
      log_file(INET_D_LOG, sprintf("GetMudFromQueue: deleted %s.\n", mud));
      packet_q -= ({ packet_q[i] });
      return GetNextMud();
    }
  }

  return 0;
}

void
eventAdvanceQueue()
{
  mixed* ml_entry;

  if(!client_busy && ml_entry = GetNextMud()) {
    string addr, mud;
    int port;

    mud  = ml_entry[0];
    addr = ml_entry[2];
    port = ml_entry[4];

#ifdef DEBUG
    DEBUG( sprintf("advQ: pulled mud from queue: mud=%s\n", mud) );
#endif

    if(client->eventCreateSocket(addr, port) >= 0) {
      conn tmp = new(conn);

      tmp->mudname = mud;                   // where we're connected to
      tmp->auth = 1;                        // always authorized here
      CONNECTION(-2) = tmp;                 // stick client into mapping

      /*
       * Initiate communication with remote mud
       */
      eventWrite(-2, ({ "oob-begin",
			mud_name(),
			1,
			AUTH_D->get_session_key(mud) }) );
      client_busy = 1;
    } else {
      log_file(INET_D_LOG,
	       sprintf("advQ: client connect to %s failed.\n", mud) );
      /*
       * retry connect in ten seconds
       */
      call_out("eventAdvanceQueue", 10);
      return;
    }
  }
#ifdef DEBUG
  else {
    if(client_busy)
      DEBUG( "advQ: Client is busy with transfer.\n" );
    else
      DEBUG( "advQ: GetNextMud returned 0 (no pending packets).\n" );
  }
#endif

  /*
   * Call out next queue advance
   */
  call_out("eventAdvanceQueue", TIME_TO_ADVANCE);
}

#ifdef DEBUG
mapping dump_conns() { return connections; }
element* dump_queue() { return copy(packet_q); }
#endif
