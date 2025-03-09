/*    /secure/daemon/finger.c
 *    from the Foundation II LPC Library
 *    gives finger information about users
 *    created by Descartes of Borg 950402
 */

#include <lib.h>
#include <daemons.h>
#include "finger.h"

inherit LIB_DAEMON;

private int LoginTime, Level;
private string CurrentUser, Short, CapName, RealName, Email;
private string *Titles;

static void create() {
    daemon::create();
    SetNoClean(1);
}

varargs string GetFinger(string who) {
    mapping mail_stat;
    object ob;
    string ret;

    if( !who ) {
	object *people;
	string *lines;
	int i, maxi;

	ret = center(mud_name(), 80) + "\n\n";
	maxi = sizeof(people = filter(users(), (: !((int)$1->GetInvis()) :)));
	lines = map(people, (: sprintf("%:-21s %:-11s", (string)$1->GetName(),
		     (string)$1->GetClass() || "No class!") :));
	ret += implode(lines, "\n");
	return ret;
    }
    if( who != CurrentUser ) {
	if( !user_exists(who) ) 
	  return capitalize(who) + " has never visited " + mud_name() + ".";
	else if( !unguarded( (: restore_object, DIR_PLAYERS "/" + who[0..0] +
			      "/" + who :)) &&
		!unguarded( (: restore_object, DIR_CRES "/" + who[0..0] +
			     "/" + who :) ) ) return 0;
	CurrentUser = who;
    }
    ret = replace_string(GetTitle(), "$N", CapName) + "\n";
    ret += sprintf("In real life: %:-30s Email: %s\n", RealName , 
		   (this_player(1) && archp(this_player(1))) ? Email : 
		    "Unlisted");
    if( ob = find_player(who) ) 
      ret += "On since " + ctime(LoginTime) + "\n";
    else ret += "Last on " + ctime(LoginTime) + "\n";
    mail_stat = (mapping)FOLDERS_D->mail_status(who);  
    if( mail_stat["unread"] ) 
      ret += CapName + " has " + consolidate(mail_stat["unread"], 
					     "an unread letter") + ".\n";
    if( unguarded( (: file_size, user_path(who) + ".project" :) ) > 0 )
      ret += "Project: " +
	unguarded( (: read_file, user_path(who) + ".project" :) ) + "\n";
    if( unguarded( (: file_size, user_path(who) + ".plan" :) ) > 0 )
      ret += "Plan:\n" +
	unguarded( (: read_file, user_path(who) + ".plan" :) ) + "\n";
    return ret;
}

mixed *GetRemoteFinger(string who) {
    mixed *tmp;

    who = convert_name(who);
    if( who != CurrentUser ) {
	if( !user_exists(who) ) return 0; 
	else if( !unguarded( (: restore_object, DIR_PLAYERS "/" + who[0..0] +
			      "/" + who :)) &&
		!unguarded( (: restore_object, DIR_CRES "/" + who[0..0] +
			     "/" + who :) ) ) return 0;
	CurrentUser = who;
    }
    tmp = ({ CapName, GetTitle(), RealName, Email, ctime(LoginTime),
	     "Not dealing here", Level, "\n" });
    return tmp;
}

string GetTitle() {
    if( Short ) return Short;
    else if( Titles && sizeof(Titles) ) return Titles[0];
    else return CapName;
}
