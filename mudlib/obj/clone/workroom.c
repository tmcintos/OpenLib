/*  -*- LPC -*-  */
//
// Copyright (C) 1996 Jason Borgmann (kyricc@inetnebr.com)
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
// -= Coded by the local idiot Kyricc=-
//

#include <mudlib.h>

inherit ROOM;

void
initialize()
{
  ::initialize();     // call to the original initialize() function

  set_light(1);       // set light level in this room to 1

  set_short("This is the generic Wizard's workroom.\n");

  set_long("This is where, as a wizard, you will do the enormous "
	   "amounts of coding you will no doubt be doing.  Feel free "
	   "to alter it any way you like.\n");

  set_exits((["start" : "/d/base/start.c",
              "void" : "/adm/obj/daemon/void.c",
	      ]));
}
