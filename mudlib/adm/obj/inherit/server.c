/*  -*- LPC -*-
 *    /lib/server.c
 *    from the Nightmare IV LPC Library
 *    a TCP server object
 *    created by Descartes of Borg 950429
 *
 *  10.23.95  Tim modified for this mudlib
 *  12.08.95  Tim modified to support MUD mode sockets
 *  02.05.96  Tim added SetSocketClosed(), Closed variable & related code
 *  03.23.96  Tim eventCreateSocket() returns fd of listening socket
 */

#include <mudlib.h>
#include <net/network.h>
#include "server.h"

inherit DAEMON;

private static int DestructOnClose;
private static function Read;
private static function Closed;
private static class server Listen;
private static mapping Sockets;

static void create() {
    daemon::create();
    Sockets = ([]);
}

int eventCreateSocket(int port, int type) {
    int x;

    if(type > STREAM) return -1;

    Listen = new(class server);
    Listen->Blocking = 0; /* servers are not blocking to start */
    x = socket_create(type, "eventServerReadCallback", 
		      "eventServerAbortCallback");
    if( x < 0 ) {
	eventSocketError("Error in socket_create().", x);
	return x;
    }
    Listen->Descriptor = x;
    x = socket_bind(Listen->Descriptor, port);
    if( x != EESUCCESS ) {
	eventClose(Listen);
	eventSocketError("Error in socket_bind().", x);
	return x;
    }
    x = socket_listen(Listen->Descriptor, "eventServerListenCallback");
    if( x != EESUCCESS ) {
	eventClose(Listen);
	eventSocketError("Error in socket_listen().", x);
	return x;
    }
    return Listen->Descriptor;
}

static void eventServerListenCallback(int fd) {
    int x;

    x = socket_accept(fd, "eventServerReadCallback", 
		      "eventServerWriteCallback");
    if( x < 0 ) {
	eventSocketError("Error in socket_accept().", x);
	return;
    }
    eventNewConnection(x);
}

static void eventServerAbortCallback(int fd) {
    if( Listen && Listen->Descriptor == fd) eventClose(Listen);
    else if( Sockets[fd] ) eventClose(Sockets[fd]);
}

static void eventServerReadCallback(int fd, mixed val) {
    if( functionp(Read) ) evaluate(Read, fd, val);
    else eventRead(fd, val);
}

static void eventRead(int fd, mixed val) { }

static void eventServerWriteCallback(int fd) {
    class server sock;
    int x;

    if( Listen && Listen->Descriptor == fd ) sock = Listen;
    else if( Sockets[fd] ) sock = (class server)Sockets[fd];
    else return;
    sock->Blocking = 0;
    if( !sock->Buffer && sock->Closing ) {
	eventClose(sock);
	return;
    }
    x = EESUCCESS;
    while( sock->Buffer && x == EESUCCESS ) {
	switch( x = socket_write(sock->Descriptor, sock->Buffer[0]) ) {
            case EESUCCESS:
	        break;
	    case EECALLBACK:
		sock->Blocking = 1;
		break;
	    case EEWOULDBLOCK: 
		call_out( (: eventServerWriteCallback :), 0, fd);
		return;
	    case EEALREADY:
		sock->Blocking = 1;
		return;
	    default:
		eventClose(sock);
		eventSocketError("Error in socket_write().", x);
		return;
	}
	if( sizeof(sock->Buffer) == 1 ) {
	    sock->Buffer = 0;
	    if( sock->Closing && !sock->Blocking ) eventClose(sock);
	}
	else sock->Buffer = sock->Buffer[1..];
    }
}

varargs void eventWrite(int fd, mixed val, int close) {
    class server sock;
    int x;

    if( Listen && Listen->Descriptor == fd ) sock = Listen;
    else if( Sockets[fd] ) sock = (class server)Sockets[fd];
    else return;
    if( sock->Buffer ) sock->Buffer += ({ val });
    else sock->Buffer = ({ val });
    sock->Closing = close;
    if( sock->Blocking ) return;
    else eventServerWriteCallback(sock->Descriptor);
}

static void eventClose(class server sock) {
    if( !sock ) return;
    if( Sockets[sock->Descriptor] ) map_delete(Sockets, sock->Descriptor);
    socket_close(sock->Descriptor);
    if(Closed)
      evaluate(Closed, sock->Descriptor);
    else
      eventSocketClosed(sock->Descriptor);
    sock = 0;
    if( DestructOnClose && sock == Listen ) Destruct();
}

static void eventSocketClosed(int fd) { }

int eventDestruct() {
  eventClose(Listen);
  return 1;
}

static void eventNewConnection(int fd) {
    Sockets[fd] = new(class server);
    ((class server)Sockets[fd])->Descriptor = fd;
    ((class server)Sockets[fd])->Blocking = 0; 
}

static void eventSocketError(string str, int x) { }

function SetRead(function f) { return (Read = f); }

function SetSocketClosed(function f) { return (Closed = f); }

int SetDestructOnClose(int x) { return (DestructOnClose = x); }
