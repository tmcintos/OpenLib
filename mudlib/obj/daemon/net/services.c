/*    /daemon/services.c
 *    from the Nightmare IV LPC Library
 *    handles intermud services
 *    created by Descartes of Borg 950523
 */

#include <mudlib.h>

inherit DAEMON;

static void create() {
    daemon::create();
    SetNoClean(1);
}

#include "services/channel.c"
#include "services/error.c"
#include "services/finger.c"
#include "services/locate.c"
#include "services/tell.c"
#include "services/who.c"
