/*  -*- LPC -*-
 *    /daemon/services.c
 *    from the Nightmare IV LPC Library
 *    handles intermud services
 *    created by Descartes of Borg 950523
 *
 *  10.23.95  Tim modified for this mudlib
 */

#ifndef __PACKAGE_SOCKETS__
#error You should not try to load /daemon/services.c without the sockets package
#else

#include <mudlib.h>
#include <net/network.h>

inherit DAEMON;

mapping GetServices();

static void create() {
    daemon::create();
    SetNoClean(1);
}

#include "services/channel.c"
#include "services/emoteto.c"
#include "services/error.c"
#include "services/finger.c"
#include "services/locate.c"
#include "services/tell.c"
#include "services/who.c"
#include "services/auth.c"
#include "services/file.c"
#include "services/ucache.c"
#include "services/mail.c"

mapping GetServices() {
    return ([
	//    "amcp" : "1.1",
#ifdef SERVICE_AUTH
	     "auth" : 1,
#endif
#ifdef SERVICE_CHANNEL
	     "channel" : 1,
#endif
#ifdef SERVICE_EMOTETO
	     "emoteto" : 1,
#endif
#ifdef SERVICE_FILE
	     "file" : 1,
#endif
#ifdef SERVICE_FINGER
	     "finger" : 1,
#endif
#ifdef SERVICE_LOCATE
	     "locate" : 1,
#endif
#ifdef SERVICE_MAIL
	     "mail" : 1,
#endif
#ifdef SERVICE_NEWS
	     "news" : 1,
#endif
#ifdef SERVICE_TELL
	     "tell" : 1,
#endif
#ifdef SERVICE_UCACHE
	     "ucache" : 1,
#endif
#ifdef SERVICE_WHO
	     "who" : 1,
#endif
#ifdef PORT_FTP
	     "ftp" : PORT_FTP,
#endif
#ifdef PORT_HTTP
	     "http" : PORT_HTTP,
#endif
#ifdef PORT_NNTP
	     "nntp" : PORT_NNTP,
#endif
#ifdef PORT_RCP
	     "rcp" : PORT_RCP,
#endif
#ifdef PORT_SMTP
	     "smtp" : PORT_SMTP,
#endif
	     ]);
}

#endif /* __PACKAGE_SOCKETS__ */
