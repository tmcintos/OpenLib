#include <mudlib.h>
#include <daemons.h>
#include <net/daemons.h>
mixed eval() { 
  object me = this_player();
  unguarded((: set_privs(this_object(), query_privs(this_interactive())) :), 1);
  return get_dir("/*"); }
