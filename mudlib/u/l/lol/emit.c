// emit.c
// Command that will tell_room() exactly what the player types instead of
//  parsing it like emote.c does.
//  Lol 11/04/95

#include <command.h>

int main(string str) {
  if(!str) {
    notify_fail("syntax:  emit <message>\n");
    return 0;
  }
  tell_object(this_player(), "You emit \""+str+"\" to the room.\n");
  tell_room(environment(this_player()),
    capitalize(this_player()->query_name()) + " " + str + ".\n"
    , this_player());
  return 1;
}
