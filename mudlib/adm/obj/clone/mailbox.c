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
// 09.03.96  Tim@UltraLib  Changed unread field to a general purpose flags
//                         field.  Changed behavior to mark for deletion
//                         instead of deleting immediately.

// Path to the directory under which to create mailboxes
#define MAIL_SAVE_DIR "/adm/data/user/mail"

// Define this if wanted
#undef DEBUG(x) log_file("mbox", x);

#include "mailbox.h"

private static string mailfile;   // filename of save file
private mixed* msgs;              // array of instances of class 'mmess'

#define MESG(x) ((class mmess) msgs[x])

void
create()
{
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

  // Comit changes (filter out deletions)
  msgs = filter(msgs, (: !(((class mmess) $1)->flags & F_DELETED) :));

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

  foreach(class mmess tmp in msgs) {
    if( tmp->flags & F_UNREAD )
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
  class mmess newmsg = new(class mmess);

  newmsg->subj = subj;
  newmsg->from = from;
  newmsg->mesg = mesg;
  newmsg->flags = F_UNREAD;

  msgs += ({ newmsg });
}

void
mark_del(int num)
{
  MESG(num - 1)->flags |= F_DELETED;
}

void
mark_read(int num)
{
  MESG(num - 1)->flags &= ~F_UNREAD;
}

int
is_unread(int num)
{
  return MESG(num - 1)->flags & F_UNREAD;
}

int
is_del(int num)
{
  return MESG(num - 1)->flags & F_DELETED;
}
