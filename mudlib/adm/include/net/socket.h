/*
 * socket.h:  Values used by socket efuns
 * These must correspond to the enum's in socket_efuns.h
 */

#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <net/socket.h>

/* Socket Modes */
#define MUD		0
#define STREAM		1
#define DATAGRAM	2
#define STREAM_BINARY	3
#define DATAGRAM_BINARY	4

/* Socket States */
#define CLOSED		0
#define FLUSHING	1
#define UNBOUND		2
#define BOUND		3
#define LISTEN		4
#define DATA_XFER	5

#endif /* _SOCKET_H_ */
