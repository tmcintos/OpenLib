/*  -*- LPC -*-  */
// m_weapon.c:  Functions for weapons
//
// Copyright (C) 1996 Eric Peters (epeters@netcom.com)
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
// Orig by Tim 23 Apr 96  (module created from old weapon.c)
//

#include "m_weapon.h"

//
// Global Variables
// 
private static int wielded, hands, type;
private static mixed *weapon_info;
private static mixed *wield_info;

void initialize()
{
  set_object_class(OBJECT_WEAPON | query_object_class());
  wielded = 0;
}
 
void set_wield_info(mixed *inf)
{
  wield_info = inf;
}
 
mixed *query_wield_info()
{
  return wield_info;
}
 
void set_hands(int num)
{
  hands = num;
}
 
void set_wielded(int num)
{
  wielded = num;
}
 
void set_weapon_info(mixed *weap_inf)
{
  weapon_info = weap_inf;
}
 
int query_wielded()
{
  return wielded;
}
 
mixed *query_weapon_info()
{
  return weapon_info;
}
 
int query_hands()
{
  return hands;
}
