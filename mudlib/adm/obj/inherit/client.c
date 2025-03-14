/*  -*- LPC -*-
 *    /secure/lib/client.c
 *    from the Foundation II LPC Library
 *    a TCP client object
 *    created by Descartes of Borg 950428
 *
 *  10.23.95  Tim changed to fit this mudlib
 *  02.05.96  Tim added CloseSocket()
 *            added SetSocketClose(), Close variable & related code
 *  03.25.96  Tim eventCreateSocket() returns fd on success
 */

#include <mudlib.h>
#include <net/network.h>
#include "client.h"

inherit DAEMON;

private static int DestructOnClose, SocketType = -1;
private static string LogFile;
private static function Read;
private static function Close;
private static class client Socket;

int eventCreateSocket(string host, int port) {
    int x;

    Socket = new(class client);
    Socket->Blocking = 1;
    if( SocketType == -1 ) SocketType = STREAM;
    x = socket_create(SocketType, "eventReadCallback", "eventAbortCallback");
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
		       "eventReadCallback", "eventWriteCallback");
    if( x != EESUCCESS ) {
	eventClose(Socket);
	eventSocketError("Error in socket_connect().", x);
	return x;
    }
    return Socket->Descriptor;
}

static void eventAbortCallback(int fd) {
    if( !Socket ) return;
    if( fd != Socket->Descriptor ) return;
    eventClose(Socket);
}

static void eventReadCallback(int fd, mixed val) {
    if( functionp(Read) ) evaluate(Read, val);
    else eventRead(val);
}

static void eventRead(mixed val) { }

static void eventWriteCallback(int fd) {
    int x;

    if( !Socket ) return;
    Socket->Blocking = 0;
    x = EESUCCESS;
    while( Socket->Buffer && x == EESUCCESS ) {
	switch( x = socket_write(Socket->Descriptor, Socket->Buffer[0]) ) {
            case EESUCCESS:

	        break;
	    case EECALLBACK:
		Socket->Blocking = 1;
		break;
	    case EEWOULDBLOCK:
		call_out( (: eventWriteCallback($(fd)) :), 0);
		return;
	    case EEALREADY:
		Socket->Blocking = 1;
		return;
	    default:
		eventClose(Socket);
		eventSocketError("Error in socket_write().", x);
		return;
	}
	if( sizeof(Socket->Buffer) == 1 ) Socket->Buffer = 0;
	else Socket->Buffer = Socket->Buffer[1..];
    }
}

void eventWrite(mixed val) {
    int x;

    if( !Socket ) return;
    if( Socket->Buffer ) Socket->Buffer += ({ val });
    else Socket->Buffer = ({ val });
    if( Socket->Blocking ) return;
    else eventWriteCallback(Socket->Descriptor);
}

static void eventClose(class client sock) {
    if( !sock ) return;
    socket_close(sock->Descriptor);
    sock = 0;
    if(Close)
      evaluate(Close);
    else
      eventSocketClose();
    if( DestructOnClose ) Destruct();
}

static void eventSocketClose() { }

int eventDestruct() {
    eventClose(Socket);
    return daemon::Destruct();
}

static void eventSocketError(string str, int x) { 
    if( LogFile ) 
      log_file(LogFile, ctime(time()) + "\n" + socket_error(x));
}

function SetRead(function f) { return (Read = f); }

function SetSocketClose(function f) { return (Close = f); }

int SetSocketType(int type) { return (SocketType = type); }

int SetDestructOnClose(int x) { return (DestructOnClose = x); }

void CloseSocket() { eventClose(Socket); }
