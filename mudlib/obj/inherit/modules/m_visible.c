/*  -*- LPC -*-  */
// m_visible.c:  Functions for objects which can be seen
// Tim 23 Apr 96

#include "m_visible.h"

//
// Global Variables
//
private static string *ids;              /* what we respond to */
//
// These 2 must be viewable by children since there are no query_long/short
// functions
//
static string short_desc;                /* short description */
static string long_desc;                 /* long description */

void
initialize()
{
  short_desc = long_desc = "";
  ids = ({ });
}

nomask void
set_short(string str)
{
  short_desc = str;
}

nomask void
set_long(string str)
{
  long_desc = str;
}

// An object is for all intents and purposes 'invisible' if the short
// desc. is == 0

string
short()
{
  return short_desc;
}

string
long()
{
  if( !long_desc )
    return "Up close this object still looks very nondescript.\n";
  else
    return long_desc;
}

nomask void
add_ids(string *arg)
{
  // probably want to add some security here.
  if( !arrayp(arg) || !sizeof(arg) ) return;

  ids += copy(arg);   // do we need this copy?
}

string*
query_ids()
{
  return copy(ids);
}

int
id(string arg)
{
    return (member_array(arg, ids) != -1);
}
