/* Wiz Invisibility */
/* Created by Damian */

#include <mudlib.h>
inherit DAEMON;
varargs int cmd_invis(string str) {
  if ((int)this_player()->query_invis()) {
    notify_fail("You are already invisible.\n");
    return 0;
  }
  this_player()->set_invis(1);
  message("my_action", "you disappear in a puff of smoke.",
          this_player());
          return 1;
  }
