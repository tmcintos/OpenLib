/*  -*- LPC -*-  */
// maild.c:  mail daemon.  Works in conjunction with a mail command and a
//           mailbox object.  The interface to both of these was designed
//           so you may easily write your own versions if you wish.
//
// Written 12.10.95 by Tim (astasia@iastate.edu)
//
// This file is part of the UltraLib distribution mudlib.
// It may be used and modified as long as this header remains intact.
// The UltraLib mudlib is available via FTP at knapp38.res.iastate.edu
//
// 01.21.96  Tim@UltraLib  Added features, fixed bugs.
//

#include <mudlib.h>

// Major version number changes when new features are added; minor version
// number changes with bug fixes.
#define VERSION "1.0"

// Path to your mailbox object
#define MAILBOX "/obj/clone/mailbox"

// This should be a string representing the home directory of this_player()
// with no trailing slash.  i.e. "/u/t/tim"
#define HOME_DIR(x) x->query_connection()->query_home_dir()

// Define this if your mudlib doesn't already define RESOLVE_PATH()
// It should resolve a path with respect to this_player()'s cwd.
// #define RESOLVE_PATH(x) some_function(x)

// Define this to be the filename of the editor object to use when entering
// a message.
// It should define a function as follows:
//
// void enter_text(function call_func, function abort_func, mixed* args ... );
//
// where 'call_func' is the function which will be called after the text has
// been entered, with an array (the lines of entered text) as the 1st argument
// and any extra arguments as specified by 'args'.  'abort_func' if not 0 is
// the function to call if the user aborts the editing process.
#define EDITOR "/obj/clone/editor"

// Turn on/off debugging info
#undef DEBUG

inherit DAEMON;

// Prototypes
varargs int check_mail(string user, int check_new);
void print_mesg(object mbox, int num);
void print_help();
void cmd_mail_loop(object mbox);
void cmd_send_mesg(string tolist);
void send_mail(string bcclist, string cclist, string from, string tolist, 
	       int time, string subject, string mesg);

void cmd_mail_loop2(string input, object mbox, int curr);
void cmd_send_mesg2(string input, string tolist);
void cmd_send_mesg3(string* lines, string subject, string tolist);

// This function returns # of messages if the player 'username' has mail,
// 0 if none.  If the flag 'check_new' is 1, it returns the number of
// new messages the player has.

varargs
int
check_mail(string username, int check_new)
{
  object mbox = new("/obj/clone/mailbox");
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

// show message and mark it as read

void
print_mesg(object mbox, int num) {
  write((string) mbox->get_mesg(num));
  mbox->mark_read(num);
}

void
print_help()
{
  write("Help for Mail System:\n\n"
	"?                       Show this help message.\n"
	"q                       Quit and save state of mailbox.\n"
	"Q                       Quit without saving changes to mailbox.\n"
	"h                       Print message headers.\n"
	"n                       Show next message.\n"
	"p                       Show previous message.\n"
	"d <message number>      Delete current message if no argument.\n"
	"w [filename]            Append current message to file.\n"
	"                        Appends to ~/mbox if no filename given.\n"
	);
}

//  This sequence is started by the mail command to read mail

void
cmd_mail_loop(object mbox)
{
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
  string *words = explode(input, " ");
  string cmd, savefile;
  int i;

  // 1st word is the command rest are a message list
  if(sizeof(words)) {
    cmd = words[0];
    words = words - ({ cmd });
  } else {
    cmd = "";
  }

#ifdef DEBUG
  printf("cmd=%s\n", cmd);
  write("words=" + dump_variable(words) + "\n");
#endif

  if(!count && cmd != "q" && cmd != "Q" && cmd != "?") {
    write("No messages.\n");
    write(":");
    input_to( (:cmd_mail_loop2:), 0, mbox, curr );
    return;
  }
      
  switch(cmd) {
  // save & quit
  case "q":
    mbox->save_mailbox();
    write("Saving and ");
  // quit without saving
  case "Q":
    destruct(mbox);
    write("Exiting.\n");
    return;

  // print headers
  case "h":
    printf(" %-3s     %-10s     %7s\n", "Num", "From", "Subject");
    printf("%-78'-'s\n", " ");

    for(i=1; i <= count; i++) {
      write((i==curr ? ">" : " "));
      printf("%-3d     %-10.10s     %-55.55s\n",
	     i, mbox->get_from(i), mbox->get_subj(i));
    }
    break;

  // current message
  case "":
    print_mesg(mbox, curr);
    break;

  // next message
  case "n":
    if(curr < count) {
      curr++;
      print_mesg(mbox, curr);
    } else {
      write("No next message.\n");
    }
    break;

  // previous message
  case "p":
    if(curr > 1) {
      curr--;
      print_mesg(mbox, curr);
    } else {
      write("No previous message.\n");
    }
    break;

  // delete messages
  case "d":
    if(!sizeof(words)) {
      // case no args given--delete current message
      mbox->del_mesg(curr);
      write("Deleted message.\n");
    } else {
      // case there's an argument
      sscanf(words[0], "%d", i);

      if(i > 0 && i <= count) {
	mbox->del_mesg(i);
	printf("Deleted message %d\n", i);
      } else {
	write("Invalid message number.\n");
      }
    }

    count = mbox->get_mesg_count();
    if(curr < 1)
      curr = 1;
    else if(curr > count)
      curr = count;
    break;

  // save messages to file
  case "w":
    if(!wizardp(this_player())) {
      write("Not a player command.\n");
      break;
    }

    if(!sizeof(words)) {
      savefile = HOME_DIR(this_player()) + "/mbox";
    } else {
      savefile = RESOLVE_PATH(words[0]);
    }
    write_file(savefile, (string) mbox->get_mesg(curr));
    printf("Appended message to %s.\n", savefile);
    break;
      
  // show help
  case "?":
    print_help();
    break;

  // show message by number
  default:
    if(sscanf(cmd, "%d", i) == 1) {
      if(i > 0 && i <= count) {
	curr = i;
	print_mesg(mbox, curr);
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

// This sequence is started by the mail command to create and send a message

void
cmd_send_mesg(string tolist)
{
  write("Subject: ");
  input_to((: cmd_send_mesg2 :), 0, tolist );
}

void
cmd_send_mesg2(string input, string tolist)
{
  object editor = new(EDITOR);

  editor->enter_text((: cmd_send_mesg3 :), 0, input, tolist );
}

void
cmd_send_mesg3(string* lines, string subject, string tolist)
{
  write("CC:");
  input_to((: send_mail :), 0,
	   "" , this_player()->query_cap_name(),
	   tolist, time(), subject, implode(lines, "\n")+"\n");
}

// This function distributes a mail message.
// It will eventually be modified to handle remote mail as well as local mail.
//
// 'tolist' should be a comma separated list of users to send this mesg to
// 'bcclist' should be a comma separated list of users to recieve carbon copies
// 'cclist' is for documentation only.  Messages will not be sent to users
//          in cclist.  This is for use with Intermud-3 for showing users who
//          received a carbon copy on a remote mud.
// 'time' is an integer as returned by time()
//
// The rest is self explanatory.

void
send_mail(string bcclist, string cclist, string from, string tolist, 
	  int time, string subject, string mesg)
{
  object tmpmbox = new(MAILBOX);
  string* userlist;
  string localmesg;

  bcclist = replace_string(bcclist, " ", "");
  tolist = replace_string(tolist, " ", "");
  userlist = explode(bcclist, ",") + explode(tolist, ",");

  localmesg = sprintf("To: %s\n"
		      "From: %s\n"
		      "Date: %s\n"
		      "Subject: %s\n"
		      "CC: %s\n"
		      "BCC: %s\n"
		      "-------\n%s",
		      tolist, from, ctime(time), subject,
		      cclist, bcclist, mesg);

  foreach(string user in userlist) {
    object ob;

    if(!user_exists(user)) /* no such user or remote */ {

      if(strsrch(user, '@')) {

	/* Do I3 stuff here */

	if(this_player())
	  printf("%s: intermud mail not implemented.\n", user);
      } else {
	if(this_player())
	  printf("%s: no such user.\n", user);
      }
      continue;

    } else /* local user */ {

      if(!tmpmbox->restore_mailbox(user)) {
	log_file("maild", sprintf("error loading mbox for %s\n", user));

	// Couldn't load an existing mailbox for this user.
	// We need to do this if the box doesn't load so the messages in
	// the previous box aren't still hanging around.
	tmpmbox->reinit_mailbox(user);
      }

      tmpmbox->add_mesg(subject, from, localmesg);

      if(!tmpmbox->save_mailbox()) {
	log_file("maild", sprintf("error saving mbox for %s\n", user));
	if(this_player())
	  printf("%%^RED%%^Failed%%^RESET%%^ to send mail to %s.\n", user);
      } else if(this_player()) {
	printf("Mail %%^GREEN%%^sent%%^RESET%%^ to %s.\n", user);

	if(ob = find_player(user))
	  tell_object(ob, "%^BELL%^You have %^GREEN%^NEW%^RESET%^ mail.\n");
      }

    }
  }

  destruct(tmpmbox);
}
