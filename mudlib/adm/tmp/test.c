#include <daemons.h>

void
eval()
{
  mixed* x = values(TERMCAP_D->query_all_termcap_entries());
  mapping ret = ([]);
  
  foreach(mapping tc in x)
    ret += tc;
  
  write_file("/tmp/tc", dump_variable(sort_array(keys(ret), 0)), 1);
}
