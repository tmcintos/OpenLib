/*  -*- LPC -*-  */
// room.c:   Greg@everywhere  I won't claim this...its going to be really
//           cheap right now :)
//
//  06/26/95  Tim McIntosh:   Added exits. :)  Those are always nice.
//  08/24/95  Casper: Performed misc optimization and added 
//                    misc options (light, and so on.)
//                    Also suggest change short & long to 
//                    return string of the room desc, not 
//                    write it out.
 
#include <tim.h>
 
string long_desc;         /* long description */
string short_desc;        /* short description */
int light;                /* presence of light */
mapping exits;            /* hmm...who knows   */
 
void create()
{
  exits = ([]);
  long_desc ="";
  short_desc = "";
}
 
/*
//suggest we toss in a if no living here dest here.  Casper 8/24/95
void reset()
{
 
}
*/
 
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
 
//Had too many writes for me.  Cut them out to recduce CPU & 
//clean up the apearance.  Casper 8/24/95
void long()
{
  string *keyring = keys(exits), output;
  int i = sizeof(keyring);
 
  //Add a check to deep_inventory for a lightsource.  
  //Will check docs on Rift for a possible shortcuts.  Casper 8/24/95
  if(!light)
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
      while (i-- > 1)
      {
        output +=keyring[i] +", ";
      }
      output += keyring[1] +" and "+ keyring[0] +".\n";
    }
  }
  write(output);
}
 
/*  Gee, whats this do?  Casper
string brief()
{
 
}
*/
string
short() { return short_desc; }
 
void
set_long(string str)
{
  if(!str)
    long_desc = "";
  else
    long_desc = str;
}
 
void
set_short(string str)
{
  if(!str)
    short_desc = "";
  else
    short_desc = str;
}
 
void
set_exits(mapping args)
{
  exits = args;
}
 
int
move_to_room()
{
  object me, ob;
 
  me = this_player();
  ob = exits[query_verb()];
 
  me->move(ob);
  me->force_me("look");
  return 1;
}

