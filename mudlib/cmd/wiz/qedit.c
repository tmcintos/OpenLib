#include <command.h>

// A front end for Evergreen's editor.
// Change this #define:
#define EDITOR_D "/u/e/evergreen/editord"
// to where Ev's editor actually resides.

int main(string str) {
	string ttr;

	if (!str) return notify_fail("qedit: edit what?\n");
	str = RESOLVE_PATH(str);
	EDITOR_D->edit(({ this_object(), "call_back", str }),
		((ttr = read_file(str)) ? explode(ttr, "\n") : 0));
	return 1;
}

// The below must be "public"

public void call_back(string *tbuf, mixed *str) {
	if (!tbuf) return;
	write("(writing: "+str[0]+")\n");
	rm(str[0]);
	write_file(str[0], implode(tbuf, "\n")+"\n");
}

