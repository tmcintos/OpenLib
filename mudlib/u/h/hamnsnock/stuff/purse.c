#include <mudlib.h>

inherit CONTAINER;

void
setup()
{
  set_short("a purse");
  set_long("The purse is made of thick woven fabric.\n");
  add_ids(({"purse"}));

  set_weight(10); // 10 lbs.
  set_bulk(2);  // not sure of these units yet
  set_value(({ 10, 0 }));  // worth 10 silver

  set_rigid(0); // not a rigid container
  set_weight_capacity(50); // carries 50 lbs.
  set_bulk_capacity(0);    // carries infinite bulk *grin*
}
