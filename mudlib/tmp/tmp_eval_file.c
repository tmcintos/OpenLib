#include <mudlib.h>
#include <daemons.h>
#include <net/daemons.h>
#undef unguarded
mixed eval() { 
  object me = this_player();
  return efuns(); }
