// Basic weapon Inheritable.
// Orig by Casper 9/95
// 10/14/95   Tim: removed set_long/short to work with changes to OBJECT,
//                 and removed var's long and short
 
#include <mudlib.h>
#include <object_types.h>
 
inherit OBJECT;
 
private static int wielded, hands, type;
private static mixed *weapon_info;
private static mixed *wield_info;
 
void create()
{
  object::set_object_class(OBJECT_WEAPON | OBJECT_OBJECT);
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
 
string short()
{
  return short_desc+(wielded ? "(wielded)" : "");
}
 
varargs string long(int fflag)
{
  if (fflag)
    return long_desc;
  write(long_desc);
}
 
int move(mixed dest)
{
  if(wielded)
    environment()->do_unwield(this_object(),1);
  return ::move(dest);
}
 
void remove()
{
  if(wielded)
    environment()->do_unwield(this_object(),1);
  ::remove();
 
}
