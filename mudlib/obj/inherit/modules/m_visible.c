//  -*- LPC -*-
// m_visible.c:  Functions for objects which can be seen
//
// Copyright (C) 1996 Tim McIntosh (tmcintos@dm.imaginary.com)
//
// This program is part of the OpenLib Mudlib distribution; it
// is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published
// by the Free Software Foundation; either version 2 of the License,
// or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// If you acquired this program as part of the OpenLib Mudlib
// distribution, you should have received a copy of the GNU
// General Public License in the file /doc/GPL; if not,
// write to the Free Software Foundation, Inc., 675 Mass Ave,
// Cambridge, MA 02139, USA.
//
// Orig by Tim 23 Apr 96
//

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
