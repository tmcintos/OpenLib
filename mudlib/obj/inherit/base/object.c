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

#pragma save_binary

#include <object_types.h>
#include <move.h>

// set to 'private static' so that inheritor won't be able to directly
// access this variable and so that save_object() won't save it to the .o file

private static string *ids = ({});       /* what we respond to */
private static int light;                /* presence of light */
private static int *value;               /* value of this object */
private static int weight;               /* weight of this object */
private static int object_class;         /* bitstring determines obj class */
private static boolean prevent_get;      /* if 1 we can't get this obj */
private static boolean prevent_drop;     /* if 1 we can't drop this obj */

// These 2 must be viewable by children since there are no query_long/short
// functions
static string short_desc;                /* short description */
static string long_desc;                 /* long description */

// Prototypes (someone fill in the rest sometime)
int query_weight();
int query_object_class();
int set_object_class(int obj_class);
void set_prevent_get(boolean pg);
void set_prevent_drop(boolean pd);

void
create()
{
  seteuid(getuid(this_object()));

  set_object_class(OBJECT_OBJECT);
  ids = ({});
  long_desc = "";
  short_desc = 0;
  light = 0;
  weight = 0;
}

// this is called by the driver when an object is destructed
void
remove()
{
  object ob;

  foreach(ob in all_inventory(this_object())) {
    if(ob->drop())
      ob->move(environment(this_object()));
  }
  // update attributes of the container and the containees.
}

int
move(mixed dest)
{
  object from = environment(this_object());

  // add code here to prevent disallowed moves (target is full, object
  // is immovable, etc.).  Also update attributes of source and target
  // objects. Note in the master valid_override() is currently
  // set up to restrict calls to move_object() to this file only.
  

  if(!dest || !(dest->query_object_class() & OBJECT_CONTAINER))
    return MOVE_NOT_ALLOWED;

  if(dest->query_object_class() & OBJECT_LIVING &&
    !dest->add_weight(query_weight()))
    return MOVE_TOO_HEAVY;

// when this is actually done we can put this in :>
//  if(!dest->add_bulk(query_weight()))
//    return MOVE_TOO_LARGE;
	
  move_object(dest);

  return MOVE_OK;
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

nomask
void
set_ids(string *arg)
{
  // probably want to add some security here.
  if(!arg)
    ids = ({});
  ids = arg;
}

nomask
string *
query_ids()
{
  return ids;
}

int
id(string arg)
{
  if (!arg) {
    return 0;
  }
  return (member_array(arg, ids) != -1);
}

void
set_prevent_get(boolean pg)
{
  prevent_get = pg;
}

int
get()
{
  return !prevent_get;
}

void
set_prevent_drop(boolean pd)
{
  prevent_drop = pd;
}

int
drop()
{
  return !prevent_drop;
}

nomask
void
set_light(int level)
{
  light = level;
}

nomask
int
set_long(string str)
{
  long_desc = str;

  if(!long_desc)
    long_desc = "";
}

nomask
int
set_short(string str)
{
  short_desc = str;
}

nomask
int
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

varargs
mixed
long(int flag)
{
  string ret = long_desc;
  object ob, *obs = all_inventory(this_object());

  if(!ret) ret = "Up close this object still looks very nondescript.\n";

  if(obs != ({})) {
    ret += "\nIt contains:\n";
    foreach(ob in obs) {
      string tmp;

      if(tmp = ob->short())
	ret += capitalize(tmp) + "\n";
    }
  }

  if(flag) {
    return ret;
  } else {
    string *lines = explode(ret, "\n");
    string line;

    foreach(line in lines) {
      write(line +"\n");
    }
    return 1;
  }
  return 0;
}

nomask
int *
query_value()
{
  if(value == ({}) ||
     sizeof(value) != 2 ||
     (!value[0] && !value[1]))
    return 0;
  else
    return value;
}

nomask
void
set_value(int *money)
{
  value = money;        // We can do this cause it's LPC !!
}

// can be redefined, but don't.  Mainly for living.c

int
query_weight()
{
  int total_weight = weight;
  object ob;

// If we're a container, report our weight plus the weight of what we contain

  if(query_object_class() & OBJECT_CONTAINER) {
    foreach(ob in all_inventory(this_object())) {
      total_weight += ob->query_weight();
    }
  }

  return total_weight;
}

nomask
void
set_weight(int pounds)
{
  weight = pounds;
}

// Takes a bitstring as defined in <object_types.h>

nomask
void
set_object_class(int obj_class)
{
  object_class = obj_class;
}

// returns the object class bitstring

nomask
int
query_object_class()
{
  return object_class;
}
