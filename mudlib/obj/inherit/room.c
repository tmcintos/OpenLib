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

#pragma save_binary

#include <mudlib.h>
#include <object_types.h>

inherit INHERIT_DIR "/inttostr";         /* converts int's to strings */
inherit CONTAINER;

private static boolean no_attacks_here;  /* can we attack here? 1=no 0=yes */
private static mapping exits;            /* hmm...who knows */
private static mapping items;            /* sets mapping for items */

// Prototypes
void create();
int clean_up(int inh);
void init();
nomask boolean query_no_attack();
string long();
int receive_object(object ob);
nomask void set_no_attack();                     // cannot fight here
nomask void set_exits(mapping dir_dest_mapping); // dest can be obj or string 
nomask void set_items(mapping id_long_mapping);  // long can be str/function
nomask int move_to_room();            // player command
nomask string exa_item(string item);  // returns long desc of item

// Overload of default constructor
 
void
create()
{
  ::create();
  items =([]);
  exits = ([]);

  // This saves memory when a file inherits this one and then only defines
  // a create() function and nothing else. (see manpage)
  // This works because it doesn't replace until the current execution
  // is complete.
  if( replaceable(this_object()) )
    replace_program(ROOM);
}

/*
 * override of object::clean_up() which we _don't_ want to call
 * If this is a regular room and no living present...remove
 */
int
clean_up(int inherited)
{
  if(inherited)
    return 0;

  if( !sizeof(filter_array(all_inventory(this_object()), (: living :))) )
    destruct(this_object());

  return 1;
}

//Throw this into the generic move_living() func so that the 
//coders can add inits into their rooms and not worry about the 
//::init()  Casper 8/24/95

void
init()
{
  add_action("move_to_room", keys(exits));
}

// set() is below

nomask boolean
query_no_attack()
{
  return no_attacks_here;
}

// Overload of the long() func.
// Had too many writes for me.  Cut them out to recduce CPU & 
// clean up the apearance.  Casper 8/24/95

string
long()
{
  string* keyring = keys(exits);
  int i = sizeof(keyring);
  int light = query_light();
  object* obs;
  string ret;
 
  //Add a check to deep_inventory for a lightsource.  
  //Will check docs on Rift for a possible shortcuts.  Casper 8/24/95
  if( !light ) {
    obs = deep_inventory(this_object());
    foreach(object ob in obs)
      if( (light = ob->query_light()) ) break;
    if( !light ) {
      ret = "It is dark here.\n";
      return ret;
    }
  }

  if( !long_desc )
    ret = "A nondescript room\n";
  else
    ret = long_desc;
 
  // Exit viewing crap
  if( !i ) {
    ret += "  There are no obvious exits.\n";
  } else {
    if(i == 1)
    {
      ret += "  The only obvious exit is "+ keyring[0] +".\n";
    } else {
      ret += "  There are "+ int_to_word(i) +" obvious exits:  ";

      while (--i > 1)
        ret +=keyring[i] +", ";

      ret += keyring[1] +" and "+ keyring[0] +".\n";
    }
  }

  return ret;
}

/*
 * override of container::receive_object()
 */
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

nomask void
set_exits(mapping args)
{
  exits = args;
}

nomask void
set_items(mapping args)
{
  items = args;
}
 
int
move_to_room()
{
  mixed dest_ob = exits[query_verb()];

  // for relative paths
  if(stringp(dest_ob)) {
    string* destdir = explode(dest_ob, "#");
    string path = base_name(this_object());
    path = path[0..strsrch(path, '/', -1)];
    destdir[<1] = absolute_path(path, destdir[<1]);
    dest_ob = implode(destdir, "#");
  }
  return this_player()->move_player(dest_ob, query_verb());
}

string
exa_item(string str)
{
  return evaluate(items[str]);
}
