/*  -*- LPC -*-  */
// m_weapon.c:  Functions for weapons
// Tim 23 Apr 96  (module created from old weapon.c)

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
