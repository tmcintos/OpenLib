#include <mudlib.h>

inherit OBJECT;

void
create()
{
  ::create();
  set_prevent_get(1);
}

void
init()
{
  message("none", sprintf("%O\n", previous_object()), find_player("tim"));
}
