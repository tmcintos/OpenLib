#include <mudlib.h>
#include <daemons.h>
#include <net/daemons.h>

mixed
eval()
{
  function f = function() { object me = this_player(); return read_file("/README", 0, 1); };
  mixed priv = (this_interactive() ? query_privs(this_interactive()) : 0);

  return unguarded(f, priv);
}
