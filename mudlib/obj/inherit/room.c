/*  -*- LPC -*-  */
//  room.c:  basic room object for use in building areas
//    Originally written by Tim McIntosh sometime in early 6/95
//
//  06/26/95  Tim:  Added exits. :)  Those are always nice.
//  08/24/95  Casper: Performed misc optimization and added 
//                    misc options (light, and so on.)
//  10/11/95  Tim: Changed to inherit OBJECT, removed some functions defined
//                 in OBJECT as a result of this.  Cleaned up just a little.
//  10/15/95  Lol:  Added non-object items to the rooms
//            (look, but don't touch).  Added the function exa_items
//            which is called upon by the exa.c command.
//  03/08/96  Tim: exa.c is now merged with look.c, an item may have a function
//            pointer to a function of type string; prototyped all functions;
//            took out id() since it is useless in this context;
//            also took out brief() since we can use short() for that in rooms
//  04/20/96  Tim: now inherits CONTAINER.  Made the relevant code changes.
//            Changed long() to return a formatted string only.
//
#pragma save_binary
#pragma no_warnings

#include "room.h"

inherit BASE;

inherit M_CLEAN_UP;
inherit M_OBJECT_CLASS;
inherit M_FLAGS;
inherit M_LIGHT;
inherit M_VISIBLE;
inherit M_DIMENSIONS;
inherit M_CONTAINER;

//
// Global Variables
//
private static boolean no_attacks_here;  /* can we attack here? 1=no 0=yes */
private static mapping exits;            /* hmm...who knows */
private static mapping items;            /* sets mapping for items */

void
initialize()
{
  m_flags::initialize();
  m_container::initialize();
  m_visible::initialize();
  set_object_class(OBJECT_ROOM | query_object_class());
  items =([]);
  exits = ([]);

  // This saves memory when a file inherits this one and then only defines
  // a create() function and nothing else. (see manpage)
  // This works because it doesn't replace until the current execution
  // is complete.
  if( replaceable(this_object()) )
    replace_program(ROOM);
}

//Throw this into the generic move_living() func so that the 
//coders can add inits into their rooms and not worry about the 
//::init()  Casper 8/24/95

void
init()
{
  add_action("move_to_room", keys(exits));
}

//
// Overload of m_visible::long()
//
// Had too many writes for me.  Cut them out to recduce CPU & 
// clean up the apearance.  Casper 8/24/95
// cleaned up some more.  Tim 4/24/96

string
long()
{
  string* keyring = keys(exits);
  int i = sizeof(keyring), light = query_light();
  string ret;
 
  // Add a check to deep_inventory for a lightsource.  
  // Will check docs on Rift for a possible shortcuts.  Casper 8/24/95
  if( !light ) {
    foreach(object ob in deep_inventory(this_object()))
      if( (light = ob->query_light()) ) break;

    if( !light ) return "It is dark here.\n";
  }

  if( !long_desc )
    ret = "A nondescript room\n";
  else
    ret = long_desc;
 
  // Exit viewing crap
  switch( i )
  {
  case 0:
    return ret + "  There are no obvious exits.\n";
  case 1:
    return ret + "  The only obvious exit is " + keyring[0] + ".\n";
  default:
    ret += "  There are " + to_english(i) + " obvious exits:  ";

    foreach(string dir in keyring[0..<3])
      ret += dir + ", ";

    return ret + keyring[<2] + " and " + keyring[<1] + ".\n";
  }
}

//
// overload of m_container::receive_object()
//
int
receive_object(object ob)
{
  if( !query_bulk_capacity() ) return 1;

  if( query_bulk_contained() + ob->query_bulk() > query_bulk_capacity() )
    return notify_fail("There is not room in there for you!\n");

  return 1;
}

nomask void
set_no_attack()
{
  no_attacks_here = TRUE;
}

nomask boolean
query_no_attack()
{
  return no_attacks_here;
}

nomask void
set_exits(mapping args)
{
  exits = args;
}

nomask void
add_exits(mapping args)
{
  exits += args;
}

nomask void
remove_exits(string* keylist)
{
  foreach(string key in keylist) {
    map_delete(exits, key);
  }
}

int
move_to_room()
{
  string verb = query_verb();
  if( !exits[verb] ) return 0;
  return this_player()->move_player(exits[verb], verb);
}

nomask void
set_items(mapping args)
{
  items = args;
}
 
string
exa_item(string str)
{
  return evaluate(items[str]);
}
