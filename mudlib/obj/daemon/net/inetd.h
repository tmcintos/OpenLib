#ifndef _INETD_H
#define _INETD_H

class _conn {
  string mudname;
  int auth;
};

#define conn class _conn

void create();
void setup();
void eventClientRead(mixed *packet);
void eventRead(int fd, mixed *packet);
void eventWrite(int fd, mixed *packet);
void eventSendPackets(int fd, string target_mudname);
void eventAdvanceQueue();

void AddPacketToQueue(string target_mudname, mixed *packet);

#endif /* _INETD_H */
