#include <mudlib.h>
#include <daemons.h>
#include <net/daemons.h>
mixed eval() { 
  object me = this_player();
  return memory_info(find_object("obj/inherit/clean_up")); }
