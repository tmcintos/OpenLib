/*  /u/m/misery/bboard.c
 *  Made for Dysfunctional Mud, UltraLIB
 *  Created by Misery 022296
 *  Do not remove this header
 */

#include <mudlib.h>

inherit OBJECT;

static void create();
static int get();
static void init();
static int cmd_post(string subject);

private mapping Messages;

static void create() {
    object::create();
    set_short("a Bulletin Board");
    set_long("A bulletin board used for posting messages.");
}

static int get() { return 1; }

static void init() {
add_action("cmd_post", "post");
}

static int cmd_post(string subject) {
    if(!subject) {
	write("Syntax: <post SUBJECT>\n");
	return 1;
    }
}
