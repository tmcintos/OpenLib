#include <mudlib.h>
#include <daemons.h>
#include <net/daemons.h>

mixed
eval()
{
  function f = function() { object me = this_player(); return find_player("henry")->unsetenv("PATH"); };
  mixed priv = (this_interactive() ? query_privs(this_interactive()) : 0);

  return unguarded(f, priv);
}
