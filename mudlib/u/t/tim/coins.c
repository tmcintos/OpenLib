#include <mudlib.h>

inherit CLONE_DIR "/money";

void
create()
{
 ::create();
 set_money(({ 25, 6 }));
}
