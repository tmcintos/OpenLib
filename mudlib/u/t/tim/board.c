/*
 * Board Basis Inheritable
 * Copyright 1996 Cameron Kaiser "Evergreen"
 *
 * Use of this file is governed by the UltraLib license agreement.
 * Removal of this header is prohibited by law.
 * All rights reserved.
 *
 */

#include <mudlib.h>
#include <daemons.h>

#define RESTORE "/data/obj/boards"
#define MAX_POSTINGS 15

void    post_return(int thread, string *tbuf);

class post {
  string* content;
  string owner;
}
  
inherit OBJECT;

public mapping postings;
public string boardname;

void create() { ::create();
	set_prevent_get(1);
	set_ids(({ "board", "bulletin board" }));
	postings = ([ ]);
}

void set_board_name(string str) { boardname = str; }

string short() {
	if (!boardname) return "a nonfunctional board";
	return ::short(); }

varargs mixed long(int flag) {
	string ret, *threads;
	int i, j;

	ret = capitalize(short()) + ":\n\n";
	if (boardname) {
		restore_object(RESTORE+"/"+boardname);
		threads = keys(postings);
		if (!threads || !sizeof(threads))
			ret += "The board is empty.\n\n";
		else {
			for (i=0, j=sizeof(threads); i<j; i++) {
				ret +=
sprintf("[%3d] %-50s <total: %d> (%s)", (i+1), threads[i], 
	sizeof(postings[threads[i]]),
	((class post) postings[threads[i]][0])->owner); }
			ret += "\n\n";
		}
	}
	ret += "Type \"help board\" for help.\n";
	if (flag) return ret;
		else {
			string *lines = explode(ret, "\n");
			string line;

			foreach(line in lines) write(line+"\n");
			return 1;
		}
	return 0; // why, I dunno.
}

void init() {
	if (!userp(previous_object())) return;
	if (!boardname) return;
	add_action("help", "help");
	add_action("post", "post");
	add_action("read", "read");
	add_action("bnew", "new");
}
 
int help(string str) {
	if (!str || str != "board") return 0;
	write("\n"+
"Valid bulletin board commands are post <thread number>; read\n"+
"<thread number>,<message number>; new <thread name>.\n"+
	"\n");
	return 1; }

int bnew(string str) {
	if (!str) return notify_fail("Usage: new <thread name>\n");
	if (sizeof(postings) >= MAX_POSTINGS)
		map_delete(postings, keys(postings)[0]);
	postings += ([ str : ({ }) ]);
	write("Starting a new thread: "+str+"\n");
	return (int)this_object()->post(sprintf("%i",sizeof(postings))); }

int post(string str) {
	int thread;

	if (!str || !sscanf(str, "%d", thread)) return
		notify_fail("Usage: post <thread number>\n");
	if (thread < 1 || thread > sizeof(postings)) return
	        notify_fail("Invalid thread.\n");
	write("Writing to thread "+thread+".\n\n");
	EDITOR_D->edit((: post_return, thread :));
	return 1;
}

void post_return(int thread, string *tbuf) {
	class post newpost = new(class post);

	newpost->content = tbuf;
	newpost->owner = this_player()->query_name();
	postings[(keys(postings))[--thread]] += ({ newpost });
	save_object(RESTORE+"/"+boardname);
	write("\nPosted.\n"); }
 
int read(string str) {
        class post thepost;
	int one, two, i, j;

	if (!str) return notify_fail("Usage: read <thread number>\n"+
		"       or read <thread number>,<note number>\n");
	if (sscanf(str, "%d,%d", one, two) < 2)
		if (!sscanf(str, "%d", one))
			return notify_fail("read: which thread??\n");
	write("Thread name: "+(keys(postings))[(--one)]+"\n");
	thepost = postings[(keys(postings))[one]][two];
	write("Note written by: "+thepost->owner+"\n\n");
	this_player()->more(copy(thepost->content));
	return 1;
}
