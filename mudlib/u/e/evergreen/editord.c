#include <mudlib.h>

// editord.c
// 
// This is the global editor for use with bulletin boards, mail and
// the like. Not to replace ed (although who wouldn't want to replace
// it anyway??). 
//
// 960123 Evergreen created this.

#define COMMANDS ([ "h" : "help", "d" : "delete", "i" : "insert", "f" : "insert_file", "v": "view_file", "q" : "ederror" , "e" : "jump_to_end", "r" : "ruler" ])
#define RULER "     .........1.........2.........3.........4.........5.........6.........7\n"

// Some people believed the ruler superfluous, but it's really good for
// fine text formatting and stuff. ed doesn't have it at all. It also
// makes this look more professional. ;-)

inherit DAEMON;

private string *tbuf;
private function call_back_g;
private int line;

void prompt();


// Because we're using the cardinal sin, a global variable, we have
// to clone instances of this object. edit() will handle this for us.


// Function name: edit
//
// Call edit() with a function pointer pointing to the callback function.
// You can also optionally pass a string array of the buffer to start with.
// edit() will then call that function upon completion of editing with
// a string array representing the buffer. If (!tbuf || !sizeof(tbuf))
// then editing was unsuccessful or aborted.

public varargs void edit(function call_back, string *ibuf) {
	object ob;

	if (!ibuf) ibuf = ({ }); else write("Loading buffer (lines: "+
		sizeof(ibuf)+")\n\n");
	ob = clone_object(file_name(this_object()));
	if (!ob) error("Couldn't start editor (clone_object() returned 0)");
	ob->start_edit(call_back, ibuf);
}

void start_edit(function call_back, string *ibuf) {

// By this time, we can safely assume we're within a clone of the editor,
// so global variables should be no problem.

	call_back_g = call_back;
	line = 0;
	tbuf = ibuf;
	write(""+
"     QuickEditor v0.3 (Evergreen)\n"+
"     Enter ~h on a blank line for help, or ** to save buffer and quit.\n"+
	RULER+
	"");
	prompt();
}

void prompt() {
	write(((line+1)+"    ")[0..3]+">");
	input_to("line_input");
}

void line_input(string str) {
	int i;

	if (str == "~q") {
		write("(aborted!)\n");
		evaluate(call_back_g, 0);
		destruct(this_object());
		return; }
	if (str == "**" || str == ".")  {
		evaluate(call_back_g, tbuf);
		destruct(this_object());
		return; }
	if (!str || str == "") str = " ";
	if (str[0..0] != "~") {
// "Insert a line at the line position code" is here.
		if (sizeof(str) >= 2 && str[<2..<1] == "//") {

// Warning: this is overtly MudOS v21 (the <x syntax). If you're trying
// to port this, this line needs to get changed! Ditto for the evaluate()
// calls above.

			write("(ignored line)\n");
			prompt();
			return;
		}
		if (!tbuf || line == sizeof(tbuf)) {
			line++;
			tbuf += ({ str });
		} else {
			if (!line) {
				tbuf = ({ str }) + tbuf; 
				line++;
			} else 
				tbuf = tbuf[0..(line-1)] + ({ str }) +
					tbuf[(line++)..(sizeof(tbuf)-1)];
		}
		prompt();
		return;
	}
	if (str == "~") {
		write("(no ~ command given)\n");
		prompt();
		return; }
	if (sizeof(str) < 2) 
		str += "0000";
	sscanf(str[2..<0], "%d", i);
	if (!COMMANDS[str[1..1]]) {
		write("(Unknown ~ sequence! Type ~h for help.)\n");
		prompt();
		return;
	}
	call_other(this_object(), COMMANDS[str[1..1]], i, str[2..<0]);
	prompt();
}

help(int ignore1, string ignore2) {
	write(""+
"--- Help for version 0.3 QuickEditor ---\n"+
"\n"+
"Enter your text at the line prompt (e.g., 1 >>.) The text will be inserted\n"+
"at that line position. Type // at the end of a line to ignore it.\n"+
"\n"+
"Type ** or . alone on a line to exit and \"save\" your typing.\n"+
"\n"+
"The following ~-sequences must be typed alone on a line.\n"+
"~h		This help.\n"+
"~r 		Display ruler.\n"+
"~q		Abort edit.\n"+
"~v		List the text entered so far.\n"+
"~f[filename]	Insert a file at this point. THE PATH MUST BE COMPLETE.\n"+
"		e.g. ~i/players/evergreen/embarrassing.document\n"+
"~d[line]	Delete a line. e.g. ~d5\n"+
"~i[line]	Start inserting at a certain line number. e.g. ~i5\n"+
"		lets you insert starting above line 5.\n"+
"~e		Jump to the end (same as using ~i and the last line).\n"+
	"\n");
}

view_file(int ignore1, string ignore2) {
	int i, j;

	write("(viewing file)\n\n"+RULER);
	for (i=0, j=sizeof(tbuf); i<j; i++) 
		write(((i+1)+"    ")[0..3]+"|"+tbuf[i]+"\n");
}

ruler(int ignore1, string ignore2) { write(RULER); }

insert(int where, string ignore) {
	if (where > (sizeof(tbuf)+1)) {
		write("(position exceeds buffer size)\n");
		return; }
	if (!where) {
		write("(invalid insert position)\n");
		return; }
	line = --where;
}

jump_to_end(int ignore1, string ignore2) { 

// This means I don't have to write an ~a call as well to append.
// Just ~i to the line you want to insert in front of, and ~e for the end.

	write("(jumping to end)\n");
	line = sizeof(tbuf); }

delete(int where, string ignore) {
	if (where > (sizeof(tbuf)+1)) {
		write("(position exceeds buffer size)\n");
		return; }
	if (!where) {
		write("(invalid delete line)\n");
		return; }
	write("(deleted line "+where+")\n");
	where--;
	if (!where)
		tbuf = tbuf[1..<0];
	else
		if (where == sizeof(tbuf))
			tbuf = tbuf[0..<1];
		else
			tbuf = tbuf[0..(where-1)] + tbuf[(where+1)..<0];
	if (line >= where)
		line--;
}

insert_file(int ignore1, string file) {

// You should add a hook here to catch non-wizards trying to use this.
// However, since they can't have files anyway, there ought to be little
// difficulty if your master object is fairly astute.

	string s, *abuf;

	if (!file) {
		write("(insert which file?)\n");
		return; }
	if (file[0..0] != "/") {
		write("(must specify full pathname to insert file)\n");
		return; }

// You're probably asking, why didn't I do something like this_player()->
// expand_path() or something? The reason is, I don't like using a lot
// of lib-dependent lfuns, and this code is supposed to be portable.
// If you want it, fix it for yourself.

	if (file_size(file) < 1) {
		write("(nonexistent file, empty file, or no access)\n");
		return; }
	write("(loading "+file+")\n");
	s = read_file(file);
	abuf = ((s) ? explode(s,"\n") : ({ }));
	if (!abuf || !sizeof(abuf)) {
		write("(error occurred while loading file!)\n");
		return; }
	write("(inserting lines (total: "+sizeof(abuf)+"))\n");
	tbuf = tbuf[0..(line-1)] + abuf + tbuf[(line+1)..<0];
	line += sizeof(abuf);
}

ederror() {

// This is a catch-all, but it only catches one thing so far.

	error("Logic error in QuickEditor (buffer lost!)\n"+
		"Debug stack:\n\n"+
		implode(call_stack(), "\n"));
}
