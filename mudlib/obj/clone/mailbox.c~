/*  -*- LPC -*-  */
// mailbox.c: mailbox object
// Written by Tim in fall 1995
//
// This file is part of the UltraLib distribution mudlib.
// It may be used and modified as long as this header remains intact.
// The UltraLib mudlib is available via FTP at knapp38.res.iastate.edu
//
// 01.21.96  Tim@UltraLib  Added int get_new_mesg_count() and member 'unread'
//                         to the mmess class.
// 01.22.96  Tim@UltraLib  Added member 'from' and string get_from(int num)

// Path to the directory under which to create mailboxes
#define MAIL_SAVE_DIR "/data/user/mail"

// Define this if wanted
#undef DEBUG(x) log_file("mbox", x);

#include "mailbox.h"

private static string mailfile;   // filename of save file
private mixed* msgs;              // array of instances of class 'mmess'

#define MESG(x) ((mmess) msgs[x])

void
create()
{
  seteuid(getuid(this_object()));
  msgs = ({});
}

void
reinit_mailbox(string username)
{
  mailfile = MAIL_SAVE_DIR + "/" + username[0..0] + "/" + username;

#ifdef DEBUG
  DEBUG( sprintf("reinit: %s\n", mailfile) );
#endif

  msgs = ({});
}

int
restore_mailbox(string username)
{
  mailfile = MAIL_SAVE_DIR + "/" + username[0..0] + "/" + username;

#ifdef DEBUG
  DEBUG( sprintf("restore: %s\n", mailfile) );
#endif

  return restore_object(mailfile);
}

int
save_mailbox()
{
  string mdir = mailfile[0..strsrch(mailfile, '/', -1)-1];

#ifdef DEBUG
  DEBUG( sprintf("save: %s\n", mailfile) );
#endif

  if(!directory_exists(mdir)) {
#ifdef DEBUG
    DEBUG( sprintf("mkdir: %s\n", mdir) );
#endif
    mkdir(mdir);
  }

  return save_object(mailfile);
}

int
get_mesg_count()
{
  return sizeof(msgs);
}

int
get_new_mesg_count()
{
  int ret = 0;

  foreach(mmess tmp in msgs) {
    if((int) tmp->unread)
      ret++;
  }

  return ret;
}

string
get_mesg(int num)
{
  return MESG(num - 1)->mesg;
}

string
get_subj(int num)
{
  return MESG(num - 1)->subj;
}

string
get_from(int num)
{
  return MESG(num - 1)->from;
}

void
add_mesg(string subj, string from, string mesg)
{
  mmess newmsg = new(mmess);

  newmsg->subj = subj;
  newmsg->from = from;
  newmsg->mesg = mesg;
  newmsg->unread = 1;

  msgs += ({ newmsg });
}

void
del_mesg(int num)
{
  msgs = msgs - ({ msgs[num-1] });
}

void
mark_read(int num)
{
  MESG(num - 1)->unread = 0;
}

int
is_unread(int num)
{
  return MESG(num - 1)->unread;
}
