/*  -*- LPC -*-  */
// mail.c:  I3 'mail' service implementation
//
// Created by Tim@Dysfunctional Mud 3.31.96

#include <daemons.h>
#include <net/daemons.h>

#define SERVICE_MAIL

private mapping mail_ids;

private int
new_mail_id()
{
  int* ids = keys((mail_ids ? mail_ids : mail_ids = ([])));

  if( sizeof(ids) )
    return ids[<1]++;
  else
    return 0;
}

void
eventReceiveMail(string mudname, mixed* packet)
{
  if( file_name(previous_object()) != OOB_D ) return;
  MAIL_D->send_mail(packet[3], packet[4], packet[5], packet[2] +"@"+mudname,
		    packet[6], packet[7], packet[8]);
  OOB_D->OOBSendPacket(mudname, ({ "mail-ack",
				   ([ packet[1] : ({}) ]) }));
}

void
eventReceiveMailAck(mixed* packet)
{
  if( file_name(previous_object()) != OOB_D ) return;
  foreach(int id in keys((mapping) packet[1]))
    log_file("maild", sprintf("Got ACK from %s\n", mail_ids[id]));
}

void
eventSendMail(string target_mudname, string from, mapping to_list,
	      mapping cc_list, string* bcc_list, int time, string subject,
	      string contents)
{
  int id = new_mail_id();
  mail_ids[id] = target_mudname;
  OOB_D->OOBQueuePacket(target_mudname, ({ "mail",
					   id,
					   from,
					   to_list,
					   cc_list,
					   bcc_list,
					   time,
					   subject,
					   contents 
					   }) );
}
