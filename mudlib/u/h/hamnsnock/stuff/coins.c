#include <mudlib.h>

inherit CLONE_DIR "/money";

void
create()
{
 ::create();
   set_short("magical pile of coins");
   set_money(({ 100, 100 }));
}
