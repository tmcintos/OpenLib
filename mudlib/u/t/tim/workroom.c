/*  -*- LPC -*-  */
#include <mudlib.h>

inherit ROOM;

string view_book();
int move_book(string str);

void
initialize()
{
  ::initialize();
  set_light(1);
  set_short("This is the generic Wizard's workroom.\n");
  set_long("This is where, as a wizard, you will do the enormous\n"+
	   "ammounts of coding you will no doubt be doing.  Feel free\n"+
	   "to alter it any way you like.\n"
	   "There is a book on a table in the corner.\n");
  set_items( ([ "book" : (: view_book :) ]) );
  set_exits( (["start" : "/d/base/start.c",
	       "void" : "/adm/obj/daemon/void.c",
	       ]) );
}

string
view_book()
{
  add_action("move_book", "move");
  return @END
It's hard to see what it the title says due to the lack of light in
this corner of the room.  Maybe you should move the book out into the
light a bit.
END;
}

int
move_book(string str)
{
  if( str != "book" )
    return notify_fail("Move what??\n");

  say(this_player()->query_cap_name() +
      " moves the book and a magic portal appears.\n");
  write("You move the book out into the light.  As you do this, a\n"
	"magic porthole appears mysteriously.\n");

  remove_action("move_book", "move");
  set_items( (["book" : "It is a boring book on magnetostatics.\n" ]) );
  add_exits( (["portal" : "/room/church"]) );
  return 1;
}
