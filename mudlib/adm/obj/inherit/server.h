#ifndef __SERVER_H__
#define __SERVER_H__

class server {
    int Descriptor;
    int Blocking;
    int Closing;
    mixed *Buffer;
}

int eventListenSocket(int port, int type);
int eventCreateSocket(string host, int port, int SocketType);
static void eventServerListenCallback(int fd);
static void eventServerAbortCallback(int fd);
static void eventServerReadCallback(int fd, mixed val);
static void eventRead(int fd, mixed val);
static void eventServerWriteCallback(int fd);
varargs void eventWrite(int fd, mixed val, int close);
static void eventClose(class server sock);
static void eventSocketClosed(int fd);
int eventDestruct();
static void eventNewConnection(int fd);
static void eventWriteError(int fd);
static void eventSocketError(string str, int x);

function SetRead(function f);
function SetSocketClosed(function f);
int SetDestructOnClose(int x);

#endif /* __SERVER_H__ */
