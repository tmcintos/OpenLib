/*  -*- LPC -*-  */
// goto.c:  Another hastily written command by Tim McIntosh

#include <command.h>
#define USAGE "goto <roomfile> or goto <object>"

int
main(string dest)
{
  object ob, me;

  me = this_player();
  ob = 0;

  if(!dest)
    return notify_fail("usage: "+ USAGE +"\n");

//  Check for file first
  if(file_size(RESOLVE_PATH(dest)) > 0 ||
     file_size(RESOLVE_PATH(dest+".c")) > 0) {
    ob = load_object(RESOLVE_PATH(dest));
    me->move(ob);
    me->force_me("look");
    return 1;
  }

//  Check for room object

  if(ob = find_object(dest)) {
    me->move(ob);
    me->force_me("look");
    return 1;
  }

//  Otherwise check for player

  if(ob = find_player(dest)) {
    me->move(environment(ob));
    me->force_me("look");
    return 1;
  }

//  Didn't find anything
  return notify_fail("goto: cannot find specified object\n");
}
