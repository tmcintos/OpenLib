#include <command.h>

int
main(string arg)
{
	say((string)this_player()->query_name()
		+ " says: " +  arg + "\n");
	write("You say: "+ arg +"\n");

	return 1;
}
