// Lol's workshop
// created this day of October Fifteen, Nineteenhundred and Ninty-Five

#include <mudlib.h>

inherit ROOM;

void create() {
::create() ;
  set_light(1);
  set_short("Lol's Mindworks");
  set_long("This giant workshop was constructed for the sole purpose of\ncreating reality.  Amidst the clutter of clay models, blueprints and sketchs, sits a\nlone desk in one corner.");
    set_items((["desk" : "a small wooden desk with a lamp, an ash tray with a pack of Lucky Strikes beside it, and several empty Mountain Dew bottles"]));
  set_exits((["reality" : "/d/base/start.c"]));
}

void init() {
  ::init();
  object str;
  if (!(present("long sword"))) {
  str=clone_object("/obj/examples/weapon.c");
    str->move(this_object());
  }
}
