/*  -*- LPC -*-  */
#ifndef _COMMAND_H
#define _COMMAND_H
// command.h   File included by all commands
//
//   This ensures that command objects are destructed after being idle, and
// the euid gets set right.
//
// NOTE: use write() with all commands to insure proper message class passed
//       to player. (write is redef'd here)

#include <mudlib.h>
#pragma save_binary

inherit DAEMON;
inherit CLEAN_UP;

#define write(x) message("system", x + "", this_player())

#endif /* _COMMAND_H */
