// Lol's workshop
// created this day of October Fifteen, Nineteenhundred and Ninty-Five

#include <mudlib.h>

inherit ROOM;

void create() {
::create() ;
  set_light(1);
  set_short("Lol's Mindworks");
  set_long("This giant workshop was constructed for the sole purpose of\ncreating reality.  Amidst the clutter of clay models, blueprints and sketchs, sits a\nlone desk in one corner.");
  set_exits((["reality" : "/d/base/start.c"]));
  set_items((["desk" : "is a simple wooden desk with an ash tray, a pack of Lucky Strikes, and several empty Mountain Dew bottles on top of it.",
             "ash tray" : "a very full ash tray"]));
  new("/obj/examples/weapon.c")->move(this_object());
}
