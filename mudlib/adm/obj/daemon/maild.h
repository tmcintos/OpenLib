/*  -*- LPC -*-  */
// maild.h: header file for maild.c, mudlib mail daemon.
//          The mail daemon works in conjunction with a mail command and a
//          mailbox object.  The interface to both of these was designed
//          so you may easily write your own versions if you wish.  It
//          also uses a global editor to get text from the player.
//
// Written 12.10.95 by Tim (astasia@iastate.edu)
//
// This file is part of the UltraLib distribution mudlib.
// It may be used and modified as long as this header remains intact.
// The UltraLib mudlib is available via FTP at knapp38.res.iastate.edu
//

#include <daemons.h>

// Major version number changes when new features are added; minor version
// number changes with bug fixes and implementation changes.
#define VERSION "1.2"

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
// varargs void edit(function call_back, string* input_buffer);
//
// where 'call_func' is the function which will be called after the text has
// been entered, with an array (the lines of entered text) as the last argument
// and any extra arguments should be included in the function pointer itself.
// If the edit failed, the buffer will be 0.
#define EDITOR EDITOR_D

// Turn on/off debugging info
#define DEBUG

/*
 * Function Prototypes
 * This returns the number of messages in user's mailbox.  If check_new==1
 * it returns the number of new messages in the box.
 */
varargs int check_mail(string user, int check_new);

/*
 * This starts the interactive mail reading loop
 */
void cmd_mail_loop(object mbox);

/*
 * This starts the interactive send-message dialog
 */
void cmd_send_mesg(string tolist, function call_func);

/*
 * This function distributes mail locally
 *
 * 'bcclist' is an array of usernames to send the message to.
 *
 * 'tolist'  These two are for documentation only, they should be mappings
 * 'cclist'  of the following format:
 *           ([ Mudname : ({ user-1, ... }), ... ])
 * 'time'    send time; an integer as returned by time()
 *
 * The rest is self explanatory.
 */
void send_mail(mapping to_list, mapping cc_list, string* bcc_list, string from,
	       int time, string subject, string content);

/*
 * Private Functions
 */

void print_message(object mbox, int num);
void print_headers(object mbox, int count, int curr);
void print_help();
void delete_message(object mbox, int count, int curr, string* args);
void save_message(object mbox, int curr, string* args);
void cmd_mail_loop2(string input, object mbox, int curr);
void cmd_send_mesg2(string input, string tolist, function cf);
void cmd_send_mesg3(string* lines, string subject, string tolist, function cf);
void cmd_send_mesg4(string cclist, string tolist, string subject,
			    string content, function cf);
private mapping filter_remote_users(string* arr);
