// ucache.c:  Daemon for I3 ucache service
// 11 Apr 1996 Tim@Dysfunctional Mud

#include <mudlib.h>
#include <dirs.h>
#include <net/daemons.h>

#define MAX_MUDS 60
#define MAX_USERS_PER_MUD 60

mapping ucache_info;  // mapping of mappings

void save_me() { save_object(DATA_DIR "/obj/daemon/ucache"); }

void
create()
{
  ucache_info = allocate_mapping(MAX_MUDS);
  restore_object(DATA_DIR "/obj/daemon/ucache", 1);
}

int
remove()
{
  save_me();
  return 0;
}

mapping
dump_cache()
{
  return copy(ucache_info);
}

void
cache(string mud, string user, string visname, int gender)
{
  if( file_name(previous_object()) != SERVICES_D ) return;

  if( !ucache_info[mud] ) {
    if( sizeof(ucache_info) == MAX_MUDS )
      map_delete(ucache_info, keys(ucache_info)[0]);
    ucache_info[mud] = allocate_mapping(MAX_USERS_PER_MUD);
  }
  if( sizeof(ucache_info[mud]) == MAX_USERS_PER_MUD )
    map_delete(ucache_info[mud], keys(ucache_info[mud])[0]);
  ucache_info[mud][user] = ({ visname, gender });
  save_me();
}

string*
get_info(string mud, string user)
{
  if( ucache_info[mud] )
    return copy(ucache_info[mud][user]);
  else
    return 0;
}
