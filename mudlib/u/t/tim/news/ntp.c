/* Eureka's boardless news system. Here's how it works (only through rn or 
	the rn tester (rnt)).

   rn gets the newsgroup list (if valid_ntp_call() lets it).
   rn lets you read it (when time_out is clear).
   Then, you write back. While writes are in progress, time_out is set.
       You can control the max postings to a newsgroup below.

It's cheezy, but I like it better than b-boards (pukeorama).
Eureka */

#include <uid.h>
#include <mudlib.h>
#define MAX_POSTINGS 29

inherit DAEMON;

int time_out;
string *newsgroups;

create() { ::create();
	seteuid(ROOT_UID);
	time_out = 0; /* don't lock the news read */
	newsgroups = explode(read_file("/ntp/master"),"\n");
	}

nomask int valid_ntp_call() {
	if (geteuid(previous_object()) == ROOT_UID) return 1;
		// after all, no joe-blo object will have ROOT anyway.
	if (previous_object() == this_object()) return 1; // shrug again.
	if (!find_object("/adm/obj/rn_control"))
		return 0; /* not loaded, therefore not used */
	if (member_array(previous_object(), children("/adm/obj/rn_control"))
		== -1)
		return 0;
	return 1; }

// I wonder whether these should be nomasks, too?

string *query_groups() {
	return ((valid_ntp_call()) ? newsgroups : ({ })); }

string get_group_file(string group) {
	if (!valid_ntp_call()) return 0;
	if (time_out) return "busy";
	if (member_array(group,newsgroups) == -1) return 0;
	if (file_size("/ntp/"+group) == -1) return "ff";
	return read_file("/ntp/"+group); }

int write_group_file(string message, string group) {
	string current;
	int i, j;

	if (!valid_ntp_call()) return 0;
	if (!(current = get_group_file(group))) return 0;
	if (current == "busy") return -2;
	
	time_out = 1;
	current += message;
	i = sizeof(explode(current, "**\n"))-1;
	j = i - MAX_POSTINGS;
	
	current = implode(explode(current,"**\n")[j..i],"**\n")+"**\n";
	if (!rm("/ntp/"+group)) { time_out = 0; return -1; } 
	if (!write_file("/ntp/"+group, current)) { time_out = 0; return -1; }
		else { time_out = 0; return 1; }
}

