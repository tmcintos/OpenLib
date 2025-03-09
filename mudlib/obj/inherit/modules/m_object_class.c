//  -*- LPC -*-
// m_object_class.c:  Functions for setting/checking the type of an object
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

#include "m_object_class.h"

//
// Global Variables
//
private static int object_class;         /* bitstring determines obj class */

nomask void
set_object_class(int obj_class)
{
  object_class = obj_class;
}

nomask int
query_object_class()
{
  return object_class;
}

nomask int
query_is_class(int obj_class)
{
  return (obj_class & object_class);
}
