/*  -*- LPC -*-  */
// Written by Tim in fall 1995
//
// This file is part of the UltraLib distribution mudlib.
// It may be used and modified as long as this header remains intact.
// The UltraLib mudlib is available via FTP at knapp38.res.iastate.edu
//

class mmess {
  int flags;
  string from;
  string subj;
  string mesg;
}

#define F_UNREAD  1
#define F_DELETED 2

// Interface to mailbox object
// NOTE: messages are numbered starting with number 1
//
// MAINTENANCE
void reinit_mailbox(string username);  // initialize a mailbox to empty state
int restore_mailbox(string username);  // restore mailbox state
int save_mailbox();                    // save mailbox state (commit changes)
// INFO
int get_mesg_count();        // return number of messages in box
int get_new_mesg_count();    // return number of new messages in box
string get_mesg(int num);    // return message number 'num'
string get_subj(int num);    // return subject line for message 'num'
string get_from(int num);    // return the mail addr of the sender of msg 'num'
// ACTION
void add_mesg(string subj, string from, string mesg);  // add a new message
void mark_del(int num);      // mark message number 'num' for deletion
void mark_read(int num);     // mark a message as read
// STATUS
int is_unread(int num);      // 1 if message 'num' is unread
int is_del(int num);         // 1 if message 'num' is marked for deletion
