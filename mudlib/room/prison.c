#include <mudlib.h>
inherit ROOM;

void
create() {
  ::create();
  set_light(1);
  set_short("The local prison");
  set_long("You are in the local prison.\n");
}

void init() {
  ::init();
  add_action("quit", "quit");
}

int quit() { return 1; }
