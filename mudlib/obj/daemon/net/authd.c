/*  -*- LPC -*-  */
// authd.c:  authentication daemon
//   keeps track of session keys for this mud and to other muds
//
// created 12.8.95 by Tim@Dysfunctional Mud

#include <mudlib.h>
#include <net/daemons.h>
#include "authd.h"

// Debug stuff.  Does what you think it does.
#define DEBUG

// Debug file passed to log_file()
#ifdef DEBUG
#define DBFILE "authd"
#endif

inherit DAEMON;

private mapping keymap;  // mapping by mudname of keysets

void
create()
{
  ::create();
  keymap = ([]);
}

#ifdef DEBUG
mapping
dump_keymap()
{
  return keymap;
}
#endif

// clean up keymap periodically, removing muds which have expired keys here
// _and_ when our key to them has probably expired also

void
reset()
{
  int t = time();

  foreach(string mud, keyset k in keymap) {
    if(t - k->conn_there_timestamp > 600 &&
       t - k->conn_here_timestamp > 600) {
#ifdef DEBUG
	 log_file(DBFILE, sprintf("reset: deleting %s from keymap\n", mud));
#endif
	 map_delete(keymap, mud);
    }
  }
}

// generate a new key to give to a mud, and set up a call_out to expire it

int
generate_key(string mudname)
{
  int keyval;

/* code that sets keyval*/
  keyval = random(46340) * random(46340);

  if(!keymap[mudname]) {
    keyset newkey = new(keyset);

    newkey->conn_here = keyval;
    newkey->conn_here_timestamp = time();

    keymap[mudname] = newkey;
  } else {
    ((keyset) keymap[mudname])->conn_here = keyval;
    ((keyset) keymap[mudname])->conn_here_timestamp = time();
  }

#ifdef DEBUG
  log_file(DBFILE, sprintf("generate_key: %s, %d\n", mudname, keyval));
#endif

  return keyval;
}

// store a key recieved from another mud used to connect there

void
store_key(string mudname, int keyval)
{
#ifdef DEBUG
  log_file(DBFILE, sprintf("store_key: %s, %d\n", mudname, keyval));
#endif
  
  if(!keymap[mudname]) {
    keyset newkey = new(keyset);

    newkey->conn_there = keyval;
    newkey->conn_there_timestamp = time();

    keymap += ([ mudname : newkey ]);
  } else {
    ((keyset) keymap[mudname])->conn_there = keyval;
    ((keyset) keymap[mudname])->conn_there_timestamp = time();
  }
}

// Authenticate a mud that is trying to connect
// 1 = pass, 0 = fail

int
authenticate_mud(string mudname, int keyval)
{
#ifdef DEBUG
  log_file(DBFILE, sprintf("authenticate_mud: %s, %d", mudname, keyval));
#endif

  // no keyset on record
  if(!keymap[mudname])
    return 0;

  // 10min expiration check
  if(time() - ((keyset) keymap[mudname])->conn_here_timestamp > 600)
    return 0;

  // check for match
  if( ((keyset) keymap[mudname])->conn_here == keyval ) {
#ifdef DEBUG
  log_file(DBFILE, ",ret=1\n");
#endif
    return 1;
  } else {
#ifdef DEBUG
  log_file(DBFILE, ",ret=0\n");
#endif
    return 0;
  }
}

int
get_session_key(string mudname)
{
  return ((keyset) keymap[mudname])->conn_there;
}
