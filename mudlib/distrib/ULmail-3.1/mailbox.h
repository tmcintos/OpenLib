/*  -*- LPC -*-  */
/*
 * Copyright (C) 1996
 *	UltraLIB Administrators.  All rights reserved.
 *
 * 'UltraLIB Administrators' refers to the current administrators of the
 * UltraLIB Mudlib Development Project (herein UMDP).  Should the UMDP cease
 * to exist, ownership of this copyright should be reassigned by the last
 * administrators of the UMDP.  In the absence of such a reassignment,
 * ownership of this copyright will revert to Tim McIntosh.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the author nor the names of any co-contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE ULTRALIB ADMINISTRATORS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE ULTRALIB
 * ADMINISTRATORS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

// Written by Tim in fall 1995

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
