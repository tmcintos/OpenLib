#include <mudlib.h>
#include <gender.h>

inherit MONSTER "/human";

void
setup()
{
  object ob;

  set_name("candy");
  add_ids(({ "slut" , "hooker" , "candy" }));
  set_short("candy the hooker");
  set_long("She is very hot looking.  When you look at her it just pulls 
you into her.  Her eyes are the biggest weapon of all.\n");
  set_gender(GENDER_FEMALE);
  ob = new("/u/h/hamnsnock/stuff/whip.c");
  ob->move(this_object());
  do_wield(ob, 1, 0);

  ob = new("/u/h/hamnsnock/stuff/teddy.c");
  ob->move(this_object());
  do_wear(ob, 1, 0);

  ob = new("/u/h/hamnsnock/stuff/puse.c");
  ob->move(this_object());
}
