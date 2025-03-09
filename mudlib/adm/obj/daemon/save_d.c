/*  -*- LPC -*-  */
// save_d.c:  Object save validation daemon
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
//
// NOTE: this whole concept is kind of a hack...a more elegant solution to
// enable players to write /adm/data/ without being priv==1 is desirable.
//
// SAVE_D keeps track of objects that are wanting to save to privileged
// directories.  In order to get permission, these objects must have a
// priv 1 object make an unguarded() call for them to SAVE_D.
//
// SAVE_D will then tell the master object whether or not an object has
// permission to save to a certain file.

#define DEBUG

private static string* save_objs;
private static string* restore_objs;

#ifdef DEBUG
string*
dump_objs()
{
  return save_objs;
}
#endif

void
create()
{
  save_objs = ({ });
  restore_objs = ({ });
}

// Requests that SAVE_D validate the next call to valid_save() for the
// object ob

int
request_save(object ob)
{
  if( !check_priv(1) )
    return 0;
  
  save_objs += ({ file_name(ob) });
  return 1;
}

int
request_restore(object ob)
{
  if( !check_priv(1) )
    return 0;
  
  restore_objs += ({ file_name(ob) });
  return 1;
}

int
valid_save(object ob)
{
  int idx;

  if( previous_object() != master() ) return 0;

  idx = member_array(file_name(ob), save_objs);

  if( idx >= 0 )
  {
    if( !idx )
      save_objs = save_objs[1..];
    else
      save_objs = save_objs[0..idx - 1] + save_objs[idx + 1..];
    return 1;
  }

  return 0;
}

int
valid_restore(object ob)
{
  int idx;

  if( previous_object() != master() ) return 0;

  idx = member_array(file_name(ob), restore_objs);

  if( idx >= 0 )
  {
    if( !idx )
      restore_objs = restore_objs[1..];
    else
      restore_objs = restore_objs[0..idx - 1] + restore_objs[idx + 1..];
    return 1;
  }

  return 0;
}

