#ifndef __SERVER_H__
#define __SERVER_H__

class server {
    int Descriptor;
    int Blocking;
    int Closing;
    string *Buffer;
}

int eventCreateSocket(int port);
static void eventServerListenCallback(int fd);
static void eventServerAbortCallback(int fd);
static void eventServerReadCallback(int fd, string str);
static void eventRead(int fd, string str);
static void eventServerWriteCallback(int fd);
varargs void eventWrite(int fd, string str, int close);
static void eventClose(class server sock);
static void eventSocketClosed(int fd);
int eventDestruct();
static void eventNewConnection(int fd);
static void eventSocketError(string str, int x);
function SetRead(function f);
int SetDestructOnClose(int x);

#endif /* __SERVER_H__ */
