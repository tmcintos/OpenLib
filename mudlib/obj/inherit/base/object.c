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

// set to 'private static' so that inheritor won't be able to directly
// access this variable and so that save_object() won't save it to the .o file
#include <object_types.h>

private static string *ids;              /* what we respond to */
private static int light;                /* presence of light */
private static int *value;               /* value of this object */
private static int weight;               /* weight of this object */
private static int object_class;         /* bitstring determines obj class */

// These 2 must be viewable by children since there are no query_long/short
// functions
static string short_desc;                /* short description */
static string long_desc;                 /* long description */

// Prototypes (someone fill in the rest sometime)
int query_object_class();
int set_object_class(int obj_class);

void
create()
{
  seteuid(getuid(this_object()));

  set_object_class(OBJECT_OBJECT);
  ids = ({});
  long_desc = "";
  short_desc = 0;
  light = 0;
}

void
remove()
{
  // add code here to prevent unwanted destructions.
  // update attributes of the container and the containees.
  destruct(this_object());
}

int
move(mixed dest)
{
  // add code here to prevent disallowed moves (target is full, object
  // is immovable, etc.).  Also update attributes of source and target
  // objects. Note: /inherit/master/valid.c:valid_override() is currently
  // set up to restrict calls to move_object() to this file only.

  if(dest->query_object_class() & OBJECT_CONTAINER) {
    move_object(dest);
    return 1;
  } else {
    return 0;
  }
}

nomask
void
set_ids(string *arg)
{
  // probably want to add some security here.
  ids = arg;
}

int
id(string arg)
{
  if (!arg) {
    return 0;
  }
  if (!ids) return arg == "instance";        // Tim
  return (member_array(arg, ids) != -1);
}

// returns 1 if we can get this object

int
get()
{
  return 1;
}

// returns 1 if we can drop this_object

int
drop()
{
  return 1;
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
  if(!value || value == ({}) ||
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

nomask
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
