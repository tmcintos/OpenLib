#include <mudlib.h>

inherit DAEMON;

/* This is the interface to the new boardless news client.
	You can see its origins in the ob->cmd_rnt call.
		Eureka */

create() { ::create(); }

int cmd_rn(string arg) {
	object ob;
	ob = new("/adm/obj/rn_control");
	ob->move(this_player());
	ob->cmd_rnt(arg);
	return 1; }

string help_file() { return "/cmds/help/rn"; }
