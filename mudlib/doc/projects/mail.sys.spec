Mail System Spec     (Started by Tim 11/16/95)
----------------
[coding won't begin until this spec is complete]

Last modified:  11/16/95
by:  Tim

Functionality:
--------------
  The mail system should be able to handle mail between players on the mud
and also mail to/from users across the Intermud3 network.

- Each player should have a mailbox that contains all the messages for that
user.  This will be implemented by creating a small mailbox object, which
will be saved/restored to a file in /data/user/mail/{a..z}/.  This mailbox
object will define several functions for adding/deleting messages, getting
messages for reading and of course save/restore functions.

- Should have some sort of mailreading command etc., maybe this along with
  mail notification (on receipt and login) should be handled by a mail
  daemon (?) [add more info here]

Interface:
---------- 
// suggested interface for mailbox object:

int restore_mailbox(string username);
int save_mailbox();

int get_mesg_count();        // first message is #1, etc.
string get_mesg(int num);    // return message number 'num'
string get_subj(int num);    // return subject line for message 'num'

void add_mesg(string subj, string mesg);  // add a new message
void del_mesg(int num);      // delete message number 'num'

Implementation:
---------------

- Here's 2 ideas for the mailbox obj, don't know which is more efficient --Tim

[Tim]--------[One implementation idea:]---------------------------------------
private:
  int count;                // message count
  mixed* msgs;              // messages array

// example message entry in msgs array:
//    msgs[x] == ({ "subject line", "message number x+1" })

[Tim]--------[Another implementation idea:]-----------------------------------
class _mailmess {
  string subj;
  string mesg;
}

// closest thing we have to typedef :)
#define mmess class _mailmess

private:
  int count;                // message count
  mmess* msgs;              // array of instances of class 'mmess'

// example mail message:
//  msgs = ({});
//  msgs += ({ new(mmess) });        // have to use new() to get a new instance
//  msgs[0]->subj == "subject line";
//  msgs[0]->mesg == "message number 0";

-------------------------------------------------------------------------------
