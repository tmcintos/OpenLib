#include <mudlib.h>
#include <daemons.h>
#include <net/daemons.h>

mixed
eval()
{
  function f = function() { object me = this_player(); return me->set_short(0); };

  return unguarded(f, query_privs(this_interactive()));
}
