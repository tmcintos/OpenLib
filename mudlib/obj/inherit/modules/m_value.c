//  -*- LPC -*-
// m_value.c:  Functions for valuable objects
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

#include <money.h>

//
// Global Variables
//
private static int* value;               /* array of size COIN_TYPES */

//
// Prototypes
//
void initialize();     // make sure to call this in your routine
int* query_value();
int set_value(int*);

void
initialize()
{
  value = allocate(COIN_TYPES);
  for(int i = 0; i < COIN_TYPES; i++)
    value[i] = 0;
}

nomask int*
query_value()
{
  return copy(value);
}

nomask int
set_value(int* money)
{
  if( sizeof(money) != COIN_TYPES ) return 0;

  for(int i = 0; i < COIN_TYPES; i++)
    value[i] = money[i];

  return 1;
}
