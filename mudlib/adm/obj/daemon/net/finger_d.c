/*  -*- LPC -*-  */
// fingerd.c:  a finger server compatible with Unix's finger program
// by Tim 2/21/96

#include <daemons.h>
#include <net/config.h>
#include <net/network.h>

inherit TCP_SERVER;

mapping input;

void create();
void setup();

void
create()
{
  server::create();
  call_out("setup", 1);
  input = ([]);
}

void
setup()
{
  eventListenSocket(mud_port() + 79, STREAM);
}

void
eventRead(int fd, string val)
{
  int idx;

  if(!val) return;

  if(!input[fd])
    input[fd] = val;
  else
    input[fd] += val;


  if( (idx = strsrch(input[fd], "\r\n")) != -1 ) {
    string user = input[fd][0..idx-1];

    if(!strlen(user)) user = 0;
// log_file("tfd", sprintf("%O %i\n", input[fd][0..idx-1], idx));
    eventWrite(fd, FINGER_D->GetFinger( user ), 1);
    map_delete(input, fd);
  }
}
