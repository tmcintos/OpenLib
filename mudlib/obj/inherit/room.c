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

#pragma save_binary

#include <mudlib.h>
#include <object_types.h>

inherit INHERIT_DIR "/inttostr";         /* converts int's to strings */
inherit OBJECT;

private static boolean no_attacks_here;  /* can we attack here? 1=no 0=yes */
private static mapping exits;            /* hmm...who knows */
private static mapping items;            /* sets mapping for items */

// Prototypes
void create();
int clean_up(int inh);
void init();
nomask boolean query_no_attack();
varargs mixed long(int flag);
nomask void set_no_attack();                     // cannot fight here
nomask void set_exits(mapping dir_dest_mapping); // dest can be obj or string 
nomask void set_items(mapping id_long_mapping);  // long can be str/function
nomask int move_to_room();            // player command
nomask string exa_item(string item);  // returns long desc of item

// Overload of default constructor
 
void
create()
{
  object::create();
  set_object_class(OBJECT_CONTAINER);
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

varargs mixed
long(int flag)
{
  string *keyring = keys(exits), output;
  int i = sizeof(keyring);
  int light = query_light();
  object* obs;
 
  //Add a check to deep_inventory for a lightsource.  
  //Will check docs on Rift for a possible shortcuts.  Casper 8/24/95
  if(!light) {
    obs = all_inventory(this_object());
    for(int j = 0; j < sizeof(obs); j++)
      obs += all_inventory(obs[j]);
    foreach(object ob in obs)
      if(light = ob->query_light()) break;
    if(!light) {
      write("It is dark here.\n");
      return;
    }
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
