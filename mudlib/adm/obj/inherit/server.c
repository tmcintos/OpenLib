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
 *  06.01.96  Tim renamed original eventCreateSocket() to eventListenSocket()
 *            and created new eventCreateSocket() from the version in client.c
 *            so it is possible to create outgoing sockets from the server
 *            also. (useful for FTP and similar servers)
 *  06.01.96  Tim added eventWriteError() for notifying when a write that is
 *            in progress fails and the socket must be closed.  This allows
 *            us to determine if the socket closed normally or with an error.
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

int eventListenSocket(int port, int SocketType) {
    int x;

    if(SocketType > STREAM) return -1;

    Listen = new(class server);
    Listen->Blocking = 0; /* servers are not blocking to start */
    x = socket_create(SocketType, "eventServerReadCallback", 
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

int eventCreateSocket(string host, int port, int SocketType) {
    class server Socket;
    int x;

    Socket = new(class server);
    Socket->Blocking = 1;
    if( !SocketType ) SocketType = STREAM;
    x = socket_create(SocketType, "eventServerReadCallback",
		      "eventServerAbortCallback");
    if( x < 0 ) {
	eventSocketError("Error in socket_create().", x);
	return x;
    }
    Socket->Descriptor = x;
    x = socket_bind(Socket->Descriptor, 0);
    if( x != EESUCCESS ) {
	eventClose(Socket);
	eventSocketError("Error in socket_bind().", x);
	return x;
    }
    x = socket_connect(Socket->Descriptor, host + " " + port, 
		       "eventServerReadCallback", "eventServerWriteCallback");
    if( x != EESUCCESS ) {
	eventClose(Socket);
	eventSocketError("Error in socket_connect().", x);
	return x;
    }
    Sockets[Socket->Descriptor] = Socket;
    return Socket->Descriptor;
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
		eventWriteError(fd);
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

static void eventWriteError(int fd) { }

static void eventSocketError(string str, int x) { }

function SetRead(function f) { return (Read = f); }

function SetSocketClosed(function f) { return (Closed = f); }

int SetDestructOnClose(int x) { return (DestructOnClose = x); }
