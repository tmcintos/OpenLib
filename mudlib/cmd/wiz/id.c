/*  -*- LPC -*-  */
#include <command.h>
#include <daemons.h>

int
main(string str)
{
  object ob, conn;

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

  conn = ob->query_connection();

  printf("priv=%s oid=%d coid=%d heart_b=%s groups=%s\n",
	 "",
	 getoid(ob),
	 (conn ? getoid(conn) : -1 ),
	 (query_heart_beat(ob) ? "on" : "off"),
	 implode((string *)GROUP_D->query_groups(ob->query_name()), ","));
  return 1;
}

