/*  -*- LPC -*-  */
#include <mudlib.h>

inherit ROOM;

/* An array with destinations and directions: "room/church", "north" ... */
string dest_dir;

/* Short description of the room */
string short_desc;

/* Long description of the room */
string long_desc;

/* Special items in the room. "table", "A nice table", "window", "A window" */
string items;

/* Fact about this room. ex: "no_fight", "no_steal" */
string property;

/* No castles are allowed to be dropped here */
int no_castle_flag;

void setup()
{
  mapping tmp = ([]); 

  set_long(long_desc);
  set_short(short_desc);
  for(int i = 1; i < sizeof(dest_dir); i += 2) {
    string* destdir = explode((string)dest_dir[i-1], "#");

    destdir[<1] = "/" + destdir[<1];
    dest_dir[i-1] = implode(destdir, "#");
    tmp[dest_dir[i]] = dest_dir[i-1];
  }
  set_exits(tmp);
}

void reset(int arg) { }

void create()
{
  ::create();
  reset(0);
  setup();
}

void move_object(mixed x, mixed y) { }
