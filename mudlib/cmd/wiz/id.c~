/*  -*- LPC -*-  */
#include <command.h>
#include <daemons.h>

int
main(string str)
{
  object ob;

  if(str && str != "") {
    if(strsrch(str, '/') != -1) 
      ob = find_object(RESOLVE_PATH(str));
    else {
      ob = find_player(str);
      if(!ob)
	ob = present(str, environment(this_player()));
      if(!ob)
	ob = present(str, this_player());
    }

    if(!ob) {
      printf("No such object found\n");
      return 1;
    }
  } else {
    ob = this_player();
  }

  printf("uid=%s euid=%s oid=%d heart_b=%s groups=%s\n",
	 getuid(ob),
	 geteuid(ob),
	 getoid(ob),
	 (query_heart_beat(ob) ? "on" : "off"),
	 implode((string *)GROUP_D->query_groups(getuid(ob)), ","));
  return 1;
}

