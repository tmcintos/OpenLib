#include <mudlib.h>
#include <daemons.h>
#include <net/daemons.h>
mixed eval() { 
  object me = this_player();
  return call_out_info(); }
