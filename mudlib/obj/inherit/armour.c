// Basic armour Inheritable.
// Orig by Casper 9/95
// 10/14/95   Tim: removed set_long/short to work with changes to OBJECT,
//                 and removed var's long and short
 
#include <mudlib.h>
#include <object_types.h>
 
inherit OBJECT;
 
private static string where,location;
private static int worn,ac_one,ac_two;
 
void create()
{
  object::set_object_class(OBJECT_ARMOUR | OBJECT_OBJECT);
  worn = 0;
  where = "";
}
 
void set_type(string ttype)
{
  location = ttype;
}
string query_type()
{
  return location;
}
void set_worn(int wear, string wheere)
{
  worn = wear;
  where = wheere;
}
 
string query_worn()
{
  return (worn ? where : "");
}
 
//Used by remove only(?)
int query_where_worn()
{
  return worn;
}
 
varargs string long(int fflag)
{
  if(fflag)
    return long_desc;
  write(long_desc);
}
 
varargs string short(int fflag)
{
  if(fflag)
    return short_desc;
  return short_desc+(worn ? "("+where+")" : "");
}
 
 
void set_ac(int ac__one,int ac__two)
{
  ac_one = (ac__one == -1 ? ac_one : ac__one);
  ac_two = (ac__two == -1 ? ac_two : ac__two);
}
 
int query_ac_one()
{
  return ac_one;
}
 
int query_ac_two()
{
  return ac_two;
}
 
int *query_ac()
{
  return ({ac_one, ac_two});
}
 
int move(mixed dest)
{
  if(where!="")
    environment()->do_remove(this_object(),1);
  return ::move(dest);
}
 
void remove()
{
  if(where!="")
    environment()->do_remove(this_object(),1);
  ::remove();
 
}
