/*
 * Shell Inheritable
 * (C)1996 Cameron Kaiser "Evergreen"
 * 
 * This code is governed by the UltraLib usage agreement.
 * Removal of this header is illegal. All rights reserved.
 *
 */

#include <mudlib.h>
#define ESC ""

mapping termcap;	/* Terminal mapping for ESC sequences */
mapping aliases;	/* Aliases */
mapping env;		/* Player environment strings */

/* You must clone this object first before you start referencing it.
   This is because it uses global variables. For example:

   object shell;

   shell = clone_object(SHELL);
 
   This insures no conflict between concurrent sessions. */

int clean_up() { return 1; } // Ha! Wrong!

string shell_id() { return "Prototype Shell"; }
// Put the name of the shell here

void create() {
	string term;

	seteuid(getuid(this_player()));
	env = (mapping)this_player()->query_all_env();
	// Get environment strings from user

	aliases = (mapping)this_player()->query_aliases();
	// Get aliases from user

	term = (string)this_player()->query_term();
	if (!term) term = "dumb";
	termcap = ([
#include <termcapm.h>
		])[term];

/* termcapm.h is in the form of a mapping of mappings, viz.

"vt100" : ([ "bold", .. ]),
"dumb" : ([ "bold" : "" , ... ]) */

	write("("+shell_id()+")\n");
}

string termcap(string message) {
	string seek;

	foreach (seek in keys(termcap)) {
		replace_string(message, "%^"+seek+"%^",
			termcap[seek]); }
}
 
string receive_message(string class, string message) {
	return termcap(message); // this shell has no class interest
}

string glob(string input) { 
	if (env["noglob"]) return input; // don't if there's noglob

	return input; } // use Tim's here

string receive_input(string input) {
	// Alias expansion here

	return glob(input); }

