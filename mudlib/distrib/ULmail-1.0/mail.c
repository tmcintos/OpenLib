// mail.c: mail command
// Written 12.10.95 by Tim (astasia@iastate.edu)
//
// This file is part of the UltraLib distribution mudlib.
// It may be used and modified as long as this header remains intact.
// The UltraLib mudlib is available via FTP at knapp38.res.iastate.edu

#include <command.h>
#include <daemons.h>

#define MAILBOX "/obj/clone/mailbox"

int
main(string str)
{
  object mbox;

  if(!str) /* read mail */ {
    mbox = new(MAILBOX);

    mbox->restore_mailbox(this_player()->query_name());

    if(!(mbox->get_mesg_count())) {
      printf("No mail for %s\n", this_player()->query_name());
      destruct(mbox);
    } else {
      MAIL_D->cmd_mail_loop(mbox);
    }
  } else /* send mail */ {
    MAIL_D->cmd_send_mesg(str);
  }

  return 1;
}
