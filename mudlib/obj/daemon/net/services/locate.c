/*  -*- LPC -*-
 *    /daemon/services/locate.c
 *    from the Nightmare IV LPC Library
 *    performs player lookups
 *    created by Descartes of Borg 950624
 *
 *  10.23.95  Tim changed to fit this mudlib
 */

#define SERVICE_LOCATE

#include <daemons.h>
#include <net/daemons.h>

void eventReceiveLocateRequest(mixed *packet) {
    object ob;

    if( file_name(previous_object()) != INTERMUD_D ) return;
    if( !(ob = find_player(packet[6])) || !ob->short() ) return;
    INTERMUD_D->eventWrite( ({ "locate-reply", 5, mud_name(), 0, packet[2], 
			       packet[3], mud_name(), 
			       (string)ob->query_cap_name() }) );
}

void eventReceiveLocateReply(mixed *packet) {
    object ob;

    if( file_name(previous_object()) != INTERMUD_D ) return;
    if( !stringp(packet[5]) || !(ob = find_player(convert_name(packet[5]))) ) 
      return;
    message("system", packet[7] + " was just located on " + packet[6] + ".\n",
	    ob);
}

void eventSendLocateRequest(string who) {
    string pl;

    if( !(pl = (string)this_player(1)->query_cap_name()) ) return;
    INTERMUD_D->eventWrite( ({ "locate-req", 5, mud_name(), pl, 0, 0, who }) );
}
