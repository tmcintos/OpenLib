#include <mudlib.h>

inherit MONSTER "/human";

void
create()
{
  object weap;
  object arm;

  ::create();
  set_name("Knight");
  set_short("Mighty knight of zargon");
  set_long("A Knight sent by Zargon to destroy all living things!\n");
  weap = new("/u/h/hamnsnock/stuff/zargon");
  weap->move(this_object());
  do_wield(weap, 1, 1);
  arm = new("/u/h/hamnsnock/stuff/mchain");
  arm->move(this_object());
  do_wear(arm, 1, 1);
}
