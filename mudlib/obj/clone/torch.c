// tim: needs work--just a tool for testing right now
// prolly use heart beats or call outs eventually

#include <mudlib.h>

inherit OBJECT;

void
create()
{
  set_short("a torch");
  set_long("This is an eternally burning torch created by Tim.\n");
  set_ids(({ "torch" }));
  set_light(1);
}
