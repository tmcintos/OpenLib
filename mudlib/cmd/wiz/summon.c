#include <command.h>

int
main(string arg)
{
  object victum;
  object past_room;

  if(!arg)
    return notify_fail("usage:  summon <name>\n");

  victum = find_player(arg);

  if(!victum)
     return notify_fail("Sorry, but that player isn't on-line right now.\n");

  if(victum == this_player())
    return notify_fail("Sorry, can't summon yourself.\n");

  if(environment(victum) == environment(this_player()))
    return notify_fail(victum->query_cap_name() +
		       " is already in the room with you.\n");

  tell_room(environment(this_player()), this_player()->query_cap_name() +
	    " waves a wand in the air, and " + victum->query_cap_name() +
	    " falls on the floor.\n");

  past_room = environment(victum);

  victum->move(environment(this_player()));
  
  tell_room(past_room, victum->query_cap_name() +
	    " disappears in a puff of blue smoke.\n");

  tell_object(victum, "All of the sudden a blue smoke surrounds you, "
	      "and when it is gone, you find youself in another room.\n");

  return 1;
}
