/*  -*- LPC -*-  */
//
// inetd.h: header file for inetd OOB server
// written by Tim
//
#include <element.h>

/*
 * Full path to the savefile (without .o extension)
 */
#define SAVE_INET_D "/tmp/inetd"

/*
 * used by log_file()
 */
#define INET_D_LOG "inetd"

/*
 * Time in seconds to wait between queue advances
 */
#define TIME_TO_ADVANCE 1800

/*
 * Define this macro for debugging
 */
#define DEBUG(x) log_file("inetd.debug", x)
// #undef DEBUG

void create();
void setup();
void eventRead(int fd, mixed *packet);
void eventWrite(int fd, mixed *packet);
void eventSocketClosed(int fd);
void eventSendPackets(int fd, string target_mudname);
boolean IsConnected(string mud);
element GetMudFromQueue(string mud);
mixed* GetNextMud();
void eventAdvanceQueue();
void AddPacketToQueue(string target_mudname, mixed *packet);

class _conn {
  string mudname;
  int auth;
}

#define conn class _conn
