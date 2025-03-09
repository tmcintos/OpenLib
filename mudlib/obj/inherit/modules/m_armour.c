// armour.c:  Functions for armour objects
// Tim 23 Apr 96 (created module from old armour.c)
//               + changed query_worn() to return 0 if not worn.

#include "m_armour.h"

//
// Global Variables
//
private static string where,location;
private static int worn,ac_one,ac_two;
 
void initialize()
{
  set_object_class(OBJECT_ARMOUR | query_object_class());
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
  if( worn )
    return where;
  return 0;
}

int query_where_worn()
{
  return worn;
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
