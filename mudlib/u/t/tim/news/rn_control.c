#include <uid.h>
#include <mudlib.h>
#include <levels.h>
#define NTP_D "/adm/daemons/ntp"
#define XWIZLEVELS (({ "coder" }) + WIZLEVELS)
#define NO_RN this_player()->delete_temp("job")

inherit SECURE_OBJECT;

/*

This rather boorish client is the controller behind NWP's revolutionarily
flat boardless fake Usenet groups. Takes advantage of ANSI; allows basic
reading of new news and is designed to be at least as unfriendly as rn. (I
take great pride in that.) NOT TMI-2 DISTRIBUTION. This code is part of NWP's
lib 0.2. (Eureka)

*/

string args, post_to, *genarray, *current_news_group;
mapping newsrc;
int wizlevel, oldvals, current_group;

void main_loop();
int get_group();
int next_unread();
void display_article(int line);
void next_reply_or_quit(string str);

create() { seteuid(ROOT_UID); set("id",({"news client"})); }

int get_group() {

// get_group() returns our wizlevel, or, that is, place within XWIZLEVELS.
// This allows certain groups to be secured, e.g. ({ admin, coder }).general.

	int t, level, i;
	level = 0;
	for (t=0, i = sizeof(XWIZLEVELS); t<i; t++) {
		if (member_group((string)this_player()->query("name"),
			XWIZLEVELS[t])) level = t; }
	return level; }

int cmd_rnt(string arg) {
	string newsfile;
	string arg1;
	int i, j;

	args = arg;
	this_player()->set_temp("job","RN");
	write("\nNWP MNTP reader (Eureka, 11 Aug 1995)\n\n");
	write("Getting your newsrc ... \n");

	wizlevel = ((!wizardp(this_player())) ? -1 :
		get_group());
	newsrc = (mapping)this_player()->query("newsrc");
	if (!newsrc) 
		write("Welcome, first time newsreader -- setting up.\n");
	write("Updating newsgroups and pointers.\n");
if (wizlevel == -1) write("You have clearance to public newsgroups only.\n");
	else
	write("Your news clearance is <"+XWIZLEVELS[wizlevel]+">.\n");
	genarray = (string *)NTP_D -> query_groups();
	for (i=0, j = sizeof(genarray); i<j; i++) {
		if (!this_player()->query("newsrc/"+genarray[i])) {
			if (wizlevel > ADMIN) write("Added: "+
				genarray[i]+"\n");
			this_player()->add("newsrc/"+genarray[i], 1); }
	}
	newsrc = (mapping)this_player()->query("newsrc");
	oldvals = 0;
	if (args == "-k") {
		write("\n\nDestroyed your newsrc. Run rn again to reconstr"+
			"uct it.\n");
		this_player()->delete("newsrc");
	NO_RN;
		return 1; }

	if ((args) && sscanf (args, "-%s %s", arg1, post_to) == 2) {
		int memxwiz;
		if ((current_group = member_array(post_to,
			keys(newsrc))) == -1) {
			write("Illegal newsgroup: "+post_to+"\n");
		NO_RN;
			return 1; }
		if ((memxwiz = member_array(explode(post_to,".")[0],XWIZLEVELS) ) > -1) 
		if (wizlevel < memxwiz) {
			write("Illegal newsgroup: "+post_to+"\n");
			NO_RN;
			return 1; }
	if (arg1 == "p") {
		if (newsrc[post_to] == -1) {
			write("Unsubscribed newsgroup: "+post_to+"\n");
			NO_RN;
			return 1; }
		next_reply_or_quit("r");
		return 1; }
	if (arg1 == "r" || arg1 == "s") {
		oldvals = newsrc[post_to]; newsrc[post_to] = 1;
		if (arg1 == "r") { write("Re-reading: "+post_to+"\n"); }
		else { write("Added: "+post_to+"\n");
			this_player()->set("newsrc",newsrc);
			destruct(this_object());
			NO_RN;
			return 1; }
	} else {
		write("Illegal option: -"+arg1+"\n");
		NO_RN;
		destruct(this_object());
		return 1; }
	}
		
	current_group = -1; 
	main_loop();
	return 1; }

void main_loop() {
#define ng current_group
// Yeah, I'm lazy.

/* This code here is extremely distasteful and aesthetically, it sucks.
   Maybe it works and maybe it don't. E. */

	string newsfile;

	current_group++; if (current_group ==
		sizeof(genarray)) {
			write("\nEnd of newsgroup search.\n");
			write("Like nn says: No News (is good news)\n");
			
			// Exit and save news at this point.

			if (oldvals) newsrc[post_to] = oldvals;
			this_player()->set("newsrc",newsrc);
			NO_RN;
			destruct(this_object());
			return; }
	if (member_array(explode(genarray[ng],".")[0],XWIZLEVELS) > -1) {
	if (wizlevel < member_array(explode(genarray[ng],".")[0],XWIZLEVELS))
			{ main_loop(); return; }
		}
	write("\n");
	newsfile = "busy";
	while (!newsfile || newsfile == "busy")
		newsfile = (string)NTP_D->get_group_file(genarray[ng]);
	if (!newsfile || newsfile == "ff") {
		write("Bad or bogus newsgroup: "+genarray[ng]+"\n");
		main_loop(); return; }
	current_news_group = explode(newsfile,"\n");
	if (!next_unread()) {
		write("No new postings: "+genarray[ng]+"\n");
		main_loop(); return; } else
	{ write("Enter "+genarray[ng]+" (new postings)? [yn?qu]: ");
		input_to("unsub_quit_or_read");
		return; }
	// If we get to this point, something wrong has happened.
	write("Fail in news reader client, group = "+genarray[ng]+".\n");
	NO_RN;
	destruct(this_object());
	return; }

int next_unread() {

// This looks for the next line in the block news feed with a time stamp
// and checks it against our last read indicator in the newsrc prop.

	int i, j, k, l;
	l = newsrc[genarray[ng]]; j = -1;
	if (l == -1) {
		write("Ignoring group: "+genarray[ng]+"\n"); return 0; }
	for (i=0, k=sizeof(current_news_group); i<k; i++) {
		if (sscanf(current_news_group[i],"Time-Stamp: %d",j) &&
			(l < j)) return i+1; }
	return 0; }

void unsub_quit_or_read(string str) {
	int i, j;
	str = lower_case(str);
	if (member_array(str, ({"y","n","q","u"})) == -1) {
		write("y=YES n=NO q=QUIT u=UNSUBSCRIBE: ");
		input_to("unsub_quit_or_read");
		return; }
	if (str == "q") {
		write("Exiting rn.\n");
		write("You have not finished reading all newsgroups.\n");
		if (oldvals) newsrc[post_to] = oldvals;
		NO_RN;
		this_player()->set("newsrc",newsrc);
		destruct(this_object());
		return; }
	if (str == "n") {
		main_loop();
		return; }
	if (str == "u") {
		newsrc[genarray[ng]] = -1;
		write("Unsubscribed: "+genarray[ng]+"\n");
		main_loop(); return; }
	i = next_unread();
	sscanf(current_news_group[i-1],"Time-Stamp: %d", j);
	if (j) newsrc[genarray[ng]] = j;
	display_article(i);
	return; }

void display_article(int line) {
	int i;
	while (i<20 && line<sizeof(current_news_group) &&
		current_news_group[line] != "**") {
		write(current_news_group[line]+"\n"); 
		line++; i++; }
	if (i<20) {
		write(inverse("\nEnd of article ("+genarray[ng]+"). [?nrq]: "));
		input_to("next_reply_or_quit");
		return; }
	write(inverse("** MORE: [cr] continue, [q] quit: "));
	input_to("oh_i_want_more",0,line);
	return; }
 
void oh_i_want_more(string str, int line) {
	write(erase_line()); 
	if (str == "q")
		line = sizeof(current_news_group);
	display_article(line);
	return; }

void next_reply_or_quit(string str) {
	string article;
	if (post_to) 
		{ ng = member_array(post_to, genarray); }
	str = lower_case(str);
	if (member_array(str, ({ "n", "r", "q" })) == -1) {
		write("n=NEXT ARTICLE r=REPLY q=QUIT THIS GROUP: ");
		input_to("next_reply_or_quit");
		return; }
	if (str == "q") {
		write("Some articles may still be unread.\n");
		main_loop();
		return; }
	if (str == "n") {
		int i, j;
		string wait;
		wait = "busy";

// Some freaky thing has been occurring that the ntpd is returning
// blank news files at odd times. This should stop that.

		while (!wait || wait == "busy")
			wait = (string)NTP_D->get_group_file(genarray[ng]);
		current_news_group = explode(wait,"\n");
		j = next_unread();
		if (!j) { write("End of articles.\n");
			  main_loop(); return; }
		sscanf(current_news_group[j-1], "Time-Stamp: %d",
			i);
		if (i) newsrc[genarray[ng]] = i;
		display_article(j);
		return; }
	article =
		"Time-Stamp: "+time()+"\n"+
		"Author: "+capitalize((string)this_player()->query("name"))+"\n"+
		"Date: "+ctime(time())+"\n"+
		"Newsgroup: "+genarray[ng]+"\n";
	write("\nYou may now post your message to "+genarray[ng]+".\n");
	write("Subject: ");
	input_to("get_subject",0,article);
	return; }

get_subject(string subject, string article) {
	write("\nEnter your message (~q to abort, ** to send entry).\n\n");
	article += "Subject: "+subject+"\n\n";
	write(article);
	input_to("get_message",0,article);
	return; }
 
get_message(string str, string article) {
	if (str == "~q") {
		write("\nAborted by user.\n");
		if (!args) next_reply_or_quit("~");
			else { NO_RN;
				destruct(this_object()); }
		return; }
	article += str+"\n";
	if (str == "**") {
		int stat;
		stat = -2;
		while (stat == -2) 
			stat = (int)NTP_D->write_group_file(article,
						genarray[ng]);
		if (stat < 1) write("UNABLE TO POST! See an admin.\n");
			else
		write("Article posted.\n");
		if (!args) next_reply_or_quit("~");
			else { NO_RN; destruct(this_object()); }
		return; }
	input_to("get_message",0,article);
	return; }


