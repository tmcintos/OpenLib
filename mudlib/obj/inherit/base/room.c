/*  -*- LPC -*-  */
//  room.c:  basic room object for use in building areas
//    Originally written by Tim McIntosh sometime in early 6/95
//
//  06/26/95  Tim McIntosh:   Added exits. :)  Those are always nice.
//  08/24/95  Casper: Performed misc optimization and added 
//                    misc options (light, and so on.)
//  10/11/95  Tim: Changed to inherit OBJECT, removed some functions defined
//                 in OBJECT as a result of this.  Cleaned up just a little.
//  10/15/95  Lol:  Added non-object items to the rooms
//            (look, but don't touch).  Added the function exa_items
//            which is called upon by the exa.c command.

#pragma save_binary

#include <mudlib.h>
#include <object_types.h>

inherit INHERIT_DIR "/inttostr";
inherit OBJECT;

boolean no_attacks_here;      /* can we attack here? 1=no 0=yes */
mapping exits;            /* hmm...who knows   */
mapping items;             /*sets mapping for items  */

// Overload of default constructor
 
void create()
{
  object::create();
  object::set_object_class(OBJECT_CONTAINER);
  items =([]);
  exits = ([]);
}

// override of object::clean_up() which we _don't_ want to call
// If this is a regular room and no living present...remove
 
int
clean_up(int inherited)
{
  if(inherited)
    return 0;

  if(filter_array(all_inventory(this_object()), (: living :)) == ({}))
    destruct(this_object());
  return 1;
}

// overload of id()
 
int id(string str)
{
  return str == "room" || str == "here";
}
 
//Throw this into the generic move_living() func so that the 
//coders can add inits into their rooms and not worry about the 
//::init()  Casper 8/24/95

void init()
{
  string *keyring = keys(exits);
  int i = sizeof(keyring);
 
  while(i--)
    add_action("move_to_room", keyring[i]);
}

// Can we fight here?  (set() for this is later)

boolean query_no_attack()
{
  return no_attacks_here;
}

// Overload of the long() func.
// Had too many writes for me.  Cut them out to recduce CPU & 
// clean up the apearance.  Casper 8/24/95

varargs
mixed long(int flag)
{
  string *keyring = keys(exits), output;
  int i = sizeof(keyring);
 
  //Add a check to deep_inventory for a lightsource.  
  //Will check docs on Rift for a possible shortcuts.  Casper 8/24/95
  if(!query_light())
  {
    write("It is dark here.\n");
    return;
  }
  if(!long_desc)
    output = "A nondescript room\n";
  else
    output = long_desc;
 
// Exit viewing crap
  if(i == 0)
  {
    output +="  There are no obvious exits.\n";
  }
  else
  {
    if(i == 1)
    {
      output +="  The only obvious exit is "+ keyring[0] +".\n";
    } 
    else
    {
      output +="  There are "+ int_to_word(i) +" obvious exits:  ";
      while (--i > 1)
      {
        output +=keyring[i] +", ";
      }
      output += keyring[1] +" and "+ keyring[0] +".\n";
    }
  }

// if flag is set, return the string
  if(flag)
    return output;

// Else write out line by line, mainly for breaking up large chunks sent
// to the player...
// I'm just recycling variables here; don't get confused :)

  keyring = explode(output, "\n");
  foreach(output in keyring)
    write(output + "\n");
}
 
/*  Gee, whats this do?  Casper
string brief()
{
 
}
*/
 
void
set_no_attack()
{
  no_attacks_here = TRUE;
}

void
set_exits(mapping args)
{
  exits = args;
}

void set_items(mapping args) {
  items=args; }
 
int
move_to_room()
{
  object me;
  string dest_ob, dest_dir, tmp;
 
  me = this_player();
  dest_ob = exits[query_verb()];

// get the direction to tack onto the out line...
  if(stringp(dest_ob) && sscanf(dest_ob, "%s#%s", tmp, dest_dir) == 1) {
    dest_ob = tmp;
  } else {
    dest_dir = query_verb();
  }

  tmp = me->query_cap_name();

  say(tmp +" moves to the "+ dest_dir +".\n");
  tell_room(dest_ob, tmp + " arrives.\n");
 
  me->move(dest_ob);
  me->force_me("look");
  return 1;
}

int
exa_item(string str)
{
  string *itm, *item_desc,tmp;
  itm=keys(items);
  item_desc=values(items);

  if (member_array(str,itm) !=-1) {
    printf("You see "+item_desc[member_array(str,itm)]+".\n");
  } else {
    write("Examine what?\n");
  }
}
