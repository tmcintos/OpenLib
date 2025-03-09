/*  -*- LPC -*-  */
// goto.c:  Another hastily written command by Tim McIntosh
//   10/15/95     Tim: added 'noise' to it

#include <command.h>
#define USAGE "goto <roomfile> or goto <object>"

int
main(string dest)
{
  object ob, me, from;

  me = this_player();
  from = environment(me);
  ob = 0;

  if(!dest)
    return notify_fail("usage: "+ USAGE +"\n");

//  Check for file first
  if(file_size(RESOLVE_PATH(dest)) > 0 ||
     file_size(RESOLVE_PATH(dest+".c")) > 0) {
    ob = load_object(RESOLVE_PATH(dest));
  }

//  Check for room object

  if(!ob) {
    if(ob = find_object(dest))
      ob = environment(ob);
  }

//  Otherwise check for player

  if(!ob) {
    if(ob = find_player(dest))
      ob = environment(ob);
  }

//  Otherwise check for living
  if(!ob) {
    if(ob = find_living(dest))
      ob = environment(ob);
  }

//  Didn't find anything
  if(!ob)
    return notify_fail("goto: cannot find specified object\n");


  if( ob == from )
    return notify_fail("You stay where you are.\n");

  if( !(me->move(ob)) ) return 0;
  me->force_me("look");

  tell_room(ob, sprintf("%s falls from the sky and lands on his head.\n",
			me->query_cap_name()), ({ me }));

  tell_room(from, sprintf("%s vanishes in a puff of smoke.\n",
			  me->query_cap_name()));

  return 1;
}
