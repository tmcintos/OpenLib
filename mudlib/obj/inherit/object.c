/*  -*- LPC -*-  */
// object.c:  _the_ base for all regular objects
//   Originally written by Tim 10/95 with a few lines from MudOS/lil's base.c
//
// keep this basic...it's inherited by a couple of other base objects:
// living.c
//
// 10/11/95     Tim: Added various things, changed things around; Also
//                   modified living.c and room.c to inherit this.
// 10/14/95     Tim: Added weight & value functions & variables
// 10/28/95     Tim: Added set_prevent_get(boolean) & set_prevent_drop(boolean)
// 01/29/96     Tim: Added object flag funcs
// 04/20/96     Tim: Changed move() to call receive_object() in the dest
//              Tim: Changed long() to return formatted string only
// 04/22/96     Tim: Changed set_ids() to add_ids()

#include <mudlib.h>
#include <object_types.h>
#include <money.h>

// set to 'private static' so that inheritor won't be able to directly
// access this variable and so that save_object() won't save it to the .o file

private static string *ids;              /* what we respond to */
private static int light;                /* presence of light */
private static int *value;               /* value of this object */
private static int weight;               /* weight of this object */
private static int bulk;                 /* bulk of this object */
private static int object_class;         /* bitstring determines obj class */
private static boolean prevent_get;      /* if 1 we can't get this obj */
private static boolean prevent_drop;     /* if 1 we can't drop this obj */

private string* flags;                   /* flags we can set */

// These 2 must be viewable by children since there are no query_long/short
// functions
static string short_desc;                /* short description */
static string long_desc;                 /* long description */

// Prototypes (someone fill in the rest sometime)
void create();
int remove();
int clean_up(int inh);
nomask int move(mixed dest);
int receive_object(object ob);

nomask void add_ids(string* ids);
nomask string* query_ids();
int id(string arg);

nomask void set_prevent_get(boolean pg);
nomask void set_prevent_drop(boolean pd);
int get();
int drop();

nomask void set_light(int lev);
nomask int query_light();

nomask void set_long(string s);
nomask void set_short(string s);

string short();    // returns a lower case short desc with no period or newline
string long();     // returns a formatted long description

nomask int query_flag(string flag);
nomask void set_flag(string flag);
nomask void clear_flag(string flag);

nomask int* query_value();
nomask int set_value(int* money);

int query_weight();
int query_base_weight();
nomask void set_weight(int lbs);
int query_bulk();
int query_base_bulk();
nomask void set_bulk(int units);

nomask int query_object_class();
nomask void set_object_class(int obj_class);

/*
 * Implementation
 */
void
create()
{
  if( clonep() ) this_object()->setup();  // for setting up clones

  set_object_class(OBJECT_OBJECT);
  ids = ({ });
  long_desc = "";
  flags = ({ });

  value = allocate(COIN_TYPES);
  for(int i = 0; i < COIN_TYPES; i++)
    value[i] = 0;
}

int
remove()
{
  return 0;
}

// if this object is lost (no environment) and not inherited, then destruct

int
clean_up(int inh)
{
  if(!inh && !environment(this_object())) {
    destruct(this_object());
    return 0;
  } else {
    return 1;
  }
}

int
move(mixed dest)
{
  object from = environment(this_object());

  // add code here to prevent disallowed moves (target is full, object
  // is immovable, etc.).  Also update attributes of source and target
  // objects. Note in the master valid_override() is currently
  // set up to restrict calls to move_object() to this file only.

  if(stringp(dest)) dest = load_object(dest);

  // receive_object() sets the fail message
  if(!dest || !(dest->receive_object(this_object()))) {
    return 0;
  } else {
    move_object(dest);
    return 1;
  }
}

int
receive_object(object ob)
{
  return notify_fail(sprintf("%s is not a container!\n", capitalize(short())));
}

nomask void
add_ids(string *arg)
{
  // probably want to add some security here.
  if( !arrayp(arg) || !sizeof(arg) ) return;

  ids += copy(arg);   // do we need this copy?
}

nomask string *
query_ids()
{
  return copy(ids);
}

int
id(string arg)
{
  if( !arg )
    return 0;
  else
    return (member_array(arg, ids) != -1);
}

nomask void
set_prevent_get(boolean pg)
{
  prevent_get = pg;
}

int
get()
{
  return !prevent_get;
}

nomask void
set_prevent_drop(boolean pd)
{
  prevent_drop = pd;
}

int
drop()
{
  return !prevent_drop;
}

nomask void
set_light(int level)
{
  light = level;
}

nomask void
set_long(string str)
{
  long_desc = str;
}

nomask void
set_short(string str)
{
  short_desc = str;
}

nomask int
query_light()
{
  return light;
}

// An object is for all intents and purposes 'invisible' if the short
// desc. is == 0

string
short()
{
  return short_desc;
}

string
long()
{
  if(!long_desc)
    return "Up close this object still looks very nondescript.\n";
  else
    return long_desc;
}

nomask int
query_flag(string flag)
{
  return member_array(flag, flags) != -1;
}

nomask void
set_flag(string flag)
{
  if(!query_flag(flag)) flags += ({ flag });
}

nomask void
clear_flag(string flag)
{
  if(query_flag(flag)) flags -= ({ flag });
}

nomask int*
query_value()
{
  return copy(value);
}

nomask int
set_value(int* money)
{
  if( sizeof(money) != COIN_TYPES ) return 0;

  for(int i = 0; i < COIN_TYPES; i++)
    value[i] = money[i];

  return 1;
}

int
query_base_weight()
{
  return weight;
}

// can be redefined, but don't.  Mainly for living.c

int
query_weight()
{
  return query_base_weight();
}

nomask void
set_weight(int pounds)
{
  weight = pounds;
}

int
query_base_bulk()
{
  return bulk;
}

int
query_bulk()
{
  return query_base_bulk();
}

nomask void
set_bulk(int units)
{
  bulk = units;
}

// Takes a bitfield as defined in <object_types.h>

nomask void
set_object_class(int obj_class)
{
  object_class = obj_class;
}

// returns the object class bitstring

nomask int
query_object_class()
{
  return object_class;
}
