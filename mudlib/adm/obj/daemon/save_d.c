/*  -*- LPC -*-  */
// save_d.c:  Object save validation daemon
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

