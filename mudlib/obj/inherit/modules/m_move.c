//  -*- LPC -*-  */
// m_move.c:  Functions for movable objects
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

#include "m_move.h"

//
// Global Variables
//
private static int prevent_get;      /* if 1 we can't get this obj */
private static int prevent_drop;     /* if 1 we can't drop this obj */

int
move(mixed dest)
{
  // add code here to prevent disallowed moves (target is full, object
  // is immovable, etc.).  Also update attributes of source and target
  // objects. Note in the master valid_override() is currently
  // set up to restrict calls to move_object() to this file only.

  if( stringp(dest) ) dest = load_object(dest);

  // receive_object() sets the fail message
  if( !dest || !(dest->receive_object(this_object())) )
  {
    return 0;
  }
  else
  {
    move_object(dest);
    return 1;
  }
}

int
receive_object(object ob)
{
  return notify_fail("That is not a container!\n");
}

nomask void
set_prevent_get(boolean pg)
{
  prevent_get = pg;
}

nomask void
set_prevent_drop(boolean pd)
{
  prevent_drop = pd;
}

int
get()
{
  return !prevent_get;
}

int
drop()
{
  return !prevent_drop;
}
