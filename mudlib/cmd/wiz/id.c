/*  -*- LPC -*-  */
#include <command.h>
#include <daemons.h>

int
main(string str)
{
  object ob;

  if(str && str != "") {
    ob = find_player(str);
    if(!ob) 
      ob = find_object(RESOLVE_PATH(str));
    if(!ob) {
      printf("No such object found\n");
      return 1;
    }
  }

  if(!ob) ob = this_player();

  printf("uid=%s euid=%s oid=%d groups=%s\n",
	 getuid(ob),
	 geteuid(ob),
	 getoid(ob),
	 implode((string *)GROUP_D->query_groups(getuid(ob)), ","));
  return 1;
}
