/*  -*- LPC -*-  */
// maild.c: mudlib mail daemon.  See maild.h for details.
//
// This file is part of the UltraLib distribution mudlib.
// It may be used and modified as long as this header remains intact.
// The UltraLib mudlib is available via FTP at knapp38.res.iastate.edu
//

#include <mudlib.h>
#include <net/daemons.h>
#include "maild.h"

inherit DAEMON;

mapping open_boxes;

#ifdef DEBUG
mapping
dump_ob()
{
  return copy(open_boxes);
}
#endif

void
create()
{
  daemon::create();
  open_boxes = ([]);
}

varargs
int
check_mail(string username, int check_new)
{
  object mbox = new(MAILBOX);
  int ret;

  mbox->restore_mailbox(username);
  
  if(!check_new) {
    ret = mbox->get_mesg_count();
  } else {
    ret = mbox->get_new_mesg_count();
  }

  destruct(mbox);
  return ret;
}

/*--------------------------- Read Loop ------------------------------------*/

void
cmd_mail_loop(object mbox)
{
  open_boxes += ([ this_player()->query_name() : mbox ]);

  printf("MUD Mail v%s  Created by Tim@UltraLib.\n\n", VERSION);
  printf("%d messages in mailbox.\n", mbox->get_mesg_count());
  write("Type ? for help\n");
  write(":");
  input_to( (:cmd_mail_loop2:), 0, mbox, 1 );
}

void
cmd_mail_loop2(string input, object mbox, int curr)
{
  int count = mbox->get_mesg_count();
  string* args = explode(input, " ");
  string cmd;
  int i;

  if(!sizeof(args)) {
    cmd = "";
  } else {
    cmd = args[0];
    args -= ({ cmd });
  }

  if(!count && cmd != "q" && cmd != "Q" && cmd != "?") {
    write("No messages.\n");
    write(":");
    input_to( (:cmd_mail_loop2:), 0, mbox, curr );
    return;
  }
      
  // This condition could exist if count==curr==3 and we deleted 3
  if(curr > count)
    curr = count;

  switch(cmd) {
  // show help
  case "?":
    print_help();
    break;

  // save & quit
  case "q":
    mbox->save_mailbox();
    write("Saving and ");

  // quit without saving
  case "Q":
    map_delete(open_boxes, this_player()->query_name());
    destruct(mbox);
    write("Exiting.\n");
    return;

  // print headers
  case "h":
    print_headers(mbox, count, curr);
    break;

  // current message
  case "":
    print_message(mbox, curr);
    break;

  // next message
  case "n":
    if(curr < count) {
      curr++;
      print_message(mbox, curr);
    } else {
      write("No next message.\n");
    }
    break;

  // previous message
  case "p":
    if(curr > 1) {
      curr--;
      print_message(mbox, curr);
    } else {
      write("No previous message.\n");
    }
    break;

  // delete messages
  case "d":
    delete_message(mbox, count, curr, args);
    break;

  // save messages to file
  case "w":
    save_message(mbox, curr, args);
    break;

  // reply to current
  case "r":
    printf("Replying to %s...\n", mbox->get_from(curr));
    cmd_send_mesg2("Re: " + mbox->get_subj(curr),
		   lower_case(mbox->get_from(curr)),
		   (: cmd_mail_loop2, "donothing", mbox, curr :));
    return;

  case "donothing":
    break;
  // show message by number
  default:
    if(sscanf(cmd, "%d", i) == 1) {
      if(i > 0 && i <= count) {
	curr = i;
	print_message(mbox, curr);
      } else {
	write("Invalid message number.\n");
      }
    } else {
      write("Unknown command.\n");
    }
  }

  write(":");
  input_to((: cmd_mail_loop2 :), 0, mbox, curr );
}

void
print_message(object mbox, int num)
{
  write((string) mbox->get_mesg(num));
  mbox->mark_read(num);                           // mark message as read
}

void
print_help()
{
  write("Help for Mail System:\n\n"
	"?                Show this help message.\n"
	"d [message #]    Delete current message if no argument.\n"
	"h                Print message headers.\n"
	"n                Show next message.\n"
	"p                Show previous message.\n"
	"q                Quit and save state of mailbox.\n"
	"Q                Quit without saving changes to mailbox.\n"
	"r                Reply to current message.\n"
	"w [filename]     Append current message to file. (default: ~/mbox)\n"
	"                 Appends to ~/mbox if no filename given.\n"
	);
}

void
print_headers(object mbox, int count, int curr)
{
  printf(" %-3s     %-10s     %7s\n", "Num", "From", "Subject");
  printf("%-78'-'s\n", " ");

  for(int i = 1; i <= count; i++) {
    write((i==curr ? ">" : " "));
    printf("%-3d    %1.1s%-10.10s     %-55.55s\n",
	   i,
	   (mbox->is_unread(i) ? "*" : " "),
	   mbox->get_from(i),
	   mbox->get_subj(i));
  }
}

void
delete_message(object mbox, int count, int curr, string* args)
{
  int i;

  if(!sizeof(args)) {
    // case no args given--delete current message
    mbox->del_mesg(curr);
    write("Deleted message.\n");
  } else {
    // case there's an argument
    if(!sscanf(args[0], "%d", i)) {
      write("Must supply a numerical argument.\n");
      return;
    }

    if(i > 0 && i <= count) {
      mbox->del_mesg(i);
      printf("Deleted message %d\n", i);
    } else {
      write("Invalid message number.\n");
    }
  }
}

void
save_message(object mbox, int curr, string* args)
{
  string savefile;

  if(!wizardp(this_player())) {
    write("Not a player command.\n");
    return;
  }
  
  if(!sizeof(args)) {
    savefile = HOME_DIR(this_player()) + "/mbox";
  } else {
    savefile = RESOLVE_PATH(args[0]);
  }

  write_file(savefile, (string) mbox->get_mesg(curr));

  printf("Appended message to %s.\n", savefile);
}

/*-------------------------- Send Dialog -----------------------------------*/

void
cmd_send_mesg(string tolist, function call_func)
{
  write("Subject: ");
  input_to((: cmd_send_mesg2 :), 0, tolist, call_func);
}

void
cmd_send_mesg2(string input, string tolist, function cf)
{
  EDITOR->edit((: cmd_send_mesg3, input, tolist, cf :));
}

void
cmd_send_mesg3(string subject, string tolist, function cf, string* lines)
{
  if(!lines) {
    write("Mail aborted.\n");
  } else {
    write("CC:");
    input_to((: cmd_send_mesg4 :), 0, tolist, subject,
	   implode(lines, "\n") + "\n", cf);
  }
}

void
cmd_send_mesg4(string cclist, string tolist, string subject,
	       string contents, function cf)
{
  mapping to_list, cc_list;
  string* tmp_tolist;
  string* tmp_cclist;
  string* userlist;
  string from = this_player(1)->query_cap_name();

  tmp_tolist = explode(tolist, ",");
  tmp_cclist = explode(cclist, ",");
  /*
   * filter remote users from tolist and cclist and create mappings
   */
  to_list = filter_remote_users(tmp_tolist);
  cc_list = filter_remote_users(tmp_cclist);

  from = sprintf("%s@%s", from, mud_name());
  foreach(string mud in distinct_array(keys(cc_list) + keys(to_list))) {
    userlist = (to_list[mud] ? to_list[mud] : ({})) +
               (cc_list[mud] ? cc_list[mud] : ({}));
    if( mud == mud_name() )
      send_mail(to_list, cc_list, userlist, from, time(), subject, contents);
    else {
      SERVICES_D->eventSendMail(from,
				to_list,
				cc_list,
				userlist,
				time(), subject, contents);
      if( this_player() ) printf("Mail queued to %s\n", mud);
    }
  }
  if( cf ) evaluate(cf);
}

/* You should pass 'arr' by reference to this (don't use copy()) */

private mapping
filter_remote_users(string* arr)
{
  mapping ret = ([]);
  
  for(int i = 0; i < sizeof(arr); i++) {
    if( strsrch(arr[i], '@') != -1 ) {
      string user, host;

      sscanf(arr[i], "%s@%s", user, host);
      user = replace_string(user, " ", "");
      if( (host = INTERMUD_D->GetMudName(host)) ) {
	if( !ret[host] )
	  ret[host]  = ({ user });
	else
	  ret[host] += ({ user });
      } else if( this_player() ) printf("%s: no such host.\n", host);
      arr -= ({ arr[i] });
    } else {
      arr[i] = replace_string(arr[i], " ", "");
      if( !ret[mud_name()] )
	ret[mud_name()] = ({ arr[i] });
      else
	ret[mud_name()] += ({ arr[i] });
    }
  }
  return ret;
}

void
send_mail(mapping to_list, mapping cc_list, string* bcc_list, string from,
	  int time, string subject, string content)
{
  string localmesg;
  object tmpmbox = new(MAILBOX);
  function collapse = function(mapping x)
                      {
			string ret = "";
			foreach(string mud, string* list in x)
			  ret += implode(list, sprintf("@%s, ", mud)) +
			         "@" + mud;
			return ret;
		      };

  localmesg = sprintf("To: %s\n"
		      "From: %s\n"
		      "Date: %s\n"
		      "Subject: %s\n"
		      "CC: %s\n"
		      "-------\n%s",
		      evaluate(collapse, to_list),
		      from, ctime(time), subject,
		      evaluate(collapse, cc_list),
		      content);

  foreach(string user in bcc_list) {
    object ob = find_player(user);
    user = lower_case(user);
    /*
     * if user is in mail right now
     */
    if( open_boxes[user] ) {
      ((object) open_boxes[user])->add_mesg(subject, from, localmesg);
      if( this_player() ) printf("Mail sent to %s.\n", user);
      if( ob )
	message("system", "%^BELL%^You have %^GREEN%^NEW%^RESET%^ mail.\n",ob);
      continue;
    }
    if( !tmpmbox->restore_mailbox(user) ) {
      log_file("maild", sprintf("error loading mbox for %s\n", user));
      /*
       * Couldn't load an existing mailbox for this user.
       * We need to do this if the box doesn't load so the messages in
       * the previous box aren't still hanging around.
       */
      tmpmbox->reinit_mailbox(user);
    }
    tmpmbox->add_mesg(subject, from, localmesg);
    if( !tmpmbox->save_mailbox() ) {
      log_file("maild", sprintf("error saving mbox for %s\n", user));
      if(this_player())
	printf("%%^RED%%^Failed%%^RESET%%^ to send mail to %s.\n", user);
    } else {
      if( this_player() ) printf("Mail sent to %s.\n", user);
      if( ob )
	message("system", "%^BELL%^You have %^GREEN%^NEW%^RESET%^ mail.\n",ob);
    }
  }
  destruct(tmpmbox);
}
