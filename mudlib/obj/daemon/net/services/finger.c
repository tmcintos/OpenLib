/*    /daemon/services/finger.c
 *    from the Nightmare IV LPC Library
 *    Intermud 3 finger service implementation
 *    created by Descartes of Borg 950624
 */

#include <daemons.h>
#include <net/daemons.h>

void eventReceiveFingerRequest(mixed *packet) {
    mixed *ret;

    if( !(ret = (mixed *)FINGER_D->GetRemoteFinger(packet[6])) ) {
        INTERMUD_D->eventWrite(({ "error", 5, mud_name(), 0, packet[2],
                                  packet[3], "unk-user",
                                  capitalize(packet[6]) + " is not involved.",
				  packet }));
	return;
    }
    ret = ({ "finger-reply", 5, mud_name(), 0, packet[2], packet[3] }) + ret;
    INTERMUD_D->eventWrite(ret);
}

void eventReceiveFingerReply(mixed *packet) {
    object ob;
    string fing;

    if( file_name(previous_object()) != INTERMUD_D ) return;
    if( !(ob = find_player(convert_name(packet[5]))) ) return;
    fing = "Finger information on " + packet[6] + " from " + packet[2] + ":\n";
    fing += packet[7] + " (" + packet[8] + ")\n";
    fing += "Email: " + (packet[9] ? packet[9] : "Confidential") + "\n";
    fing += (packet[10] ? "On since: " + packet[10] + "\n" : "\n");
    fing += "Site: " + (packet[11] ? packet[11] : "Confidential") + "\n";
    fing += (packet[13] ? packet[13] : "");
    message("system", fing, ob);
}

void eventSendFingerRequest(string who, string where) {
    string pl;

    if( !(pl = (string)this_player(1)->GetName()) ) return;
    INTERMUD_D->eventWrite( ({ "finger-req", 5, mud_name(), pl, where, 0, 
			       who }) );
}
