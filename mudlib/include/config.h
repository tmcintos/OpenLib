// config.h:  global include file
//
//   This file is _automatically_ #included in _all_ objects...
//   So you don't have to include it!
//
#ifndef _CONFIG_H
#define _CONFIG_H

#include <runtime_config.h>

// These are available for clarity.  Don't test against TRUE since LPC defines
// true to be any non-zero value.

#define TRUE  1
#define FALSE 0

// These are used as typedefs to make things more clear.

#define char int            // for read-ability
#define boolean int         // for read-ability
#define NULL     0          // for clarity
#define status int          // for backwards compat don't use

#define MUD_STATUS "pre-alpha development"
#define ADMIN_EMAIL "mud@knapp38.res.iastate.edu"

#define mudlib_name() "UltraLib"
#define mudlib_version() "0.1"
#define mud_name() get_config(__MUD_NAME__)
#define mud_port() get_config(__MUD_PORT__)
#define driver_version() __VERSION__
#define machine_os() __ARCH__

// Crap for quick n dirty compatibility with the Intermud-3 stuff
// These should be phased out.

#define convert_name(x) lower_case(replace_string(x, " ", ""))
#define identify(x) typeof(x)
#define unguarded(x) evaluate(x)

// (Tim) These were put in to make it easier to change later...

#define member_group(uid, group) GROUP_D->is_member(group, uid)

// This is used to resolve a path with respect to this_player()
#define RESOLVE_PATH(x) absolute_path(this_player()->query_cwd(), x)

#endif
