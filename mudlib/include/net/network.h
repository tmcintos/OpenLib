#ifndef __NETWORK_H 
#define __NETWORK_H 
 
#include <net/socket.h> 
#include <net/config.h>
 
#define MUD                       0 
#define STREAM                    1 
#define DATAGRAM                  2 
#define STREAM_BINARY             3
#define DATAGRAM_BINARY           4
 
#define PORT_FTP                  (query_host_port() - 1)
#define PORT_HTTP                 (query_host_port() - 5)
#define PORT_OOB                  (query_host_port() + 5)
#define PORT_RCP                  (query_host_port() - 10)
#define PORT_UDP                  (query_host_port() + 8) 

// #define INTERMUD_SERVICES          ([ "channel" : 1, "finger" : 1, "tell" : 1, "locate" : 1, "who" : 1, "rcp" : PORT_RCP, "http" : PORT_HTTP, "ftp" : PORT_FTP ])

#define INTERMUD_SERVICES          ([ "finger" : 1, "tell" : 1, "who" : 1, "ftp" : PORT_FTP ])

#define SERVER_ADDR               "199.199.122.10" 
#define SERVER_PORT               7898 
 
#define MAX_PINGS                 4 
 
#define SERVICE_UDP_STARTUP       "startup" 
#define SERVICE_UDP_SHUTDOWN      "shutdown" 
#define SERVICE_UDP_PING_Q        "ping_q" 
#define SERVICE_UDP_PING_A        "ping_a" 
#define SERVICE_UDP_SUPPORT_Q     "support_q" 
#define SERVICE_UDP_SUPPORT_A     "support_a" 
#define SERVICE_UDP_RWHO_Q        "rwho_q" 
#define SERVICE_UDP_RWHO_A        "rwho_a" 
#define SERVICE_UDP_MAIL_Q        "mail_q"
#define SERVICE_UDP_MAIL_A        "mail_a"
#define SERVICE_UDP_MUDLIST_Q     "mudlist_q" 
#define SERVICE_UDP_MUDLIST_A     "mudlist_a" 
#define SERVICE_UDP_FINGER_Q      "gfinger_q" 
#define SERVICE_UDP_FINGER_A      "gfinger_a" 
#define SERVICE_UDP_LOCATE_Q      "locate_q" 
#define SERVICE_UDP_LOCATE_A      "locate_a" 
#define SERVICE_UDP_INTERMUD      "gwizmsg" 
#define SERVICE_UDP_AFFIRMATION_A "affirmation_a" 
#define SERVICE_UDP_WARNING       "warning" 
#define SERVICE_UDP_TELL          "gtell" 
 
#define INTERMUD_CHANNELS         ({ "intercre" }) 
#define INTERCHANNELS             ([ "CREATOR": "intercre" ]) 
 
#define TCP_SERVICES              ({}) 
 
#define START_MSG sprintf("||NAME:%s||PORTUDP:%d||MUDLIB:%s||VERSION:%s\
||HOST:%s||PORT:%d||TIME:%s||DRIVER:%s", mud_name(), PORT_UDP,\
mudlib(),mudlib_version(),query_host_name(), query_host_port(), ctime(time()),\
version())
 
#endif /* __NETWORK_H */ 
