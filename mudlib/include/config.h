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

// Mudlib Global Defines (more in runtime_config.h)
// All global defines are padded with double underscores on either side
// to avoid naming conflicts.

#define __MUDLIB_NAME__ "UltraLib"
#define __MUDLIB_VERSION__ "0.1"
#define __MUD_STATUS__ "pre-alpha development"
#define __ADMIN_EMAIL__ "mud@knapp38.res.iastate.edu"

// Don't use these macros--use the #define's instead.  These are just to
// preserve some compatibility until these are phased out.

#define mudlib() __MUDLIB_NAME__
#define mudlib_version() __MUDLIB_VERSION__
#define mud_name() __MUD_NAME__
#define mud_port() __MUD_PORT__
#define version() __VERSION__

// Crap for quick n dirty compatibility with the Intermud-3 stuff
// These should be phased out.

#define MUD_STATUS __MUD_STATUS__
#define ADMIN_EMAIL __ADMIN_EMAIL__
#define convert_name(x) lower_case(replace_string(x, " ", ""))
#define identify(x) typeof(x)
#define unguarded(x) evaluate(x)

// (Tim) These were put in to make it easier to change later...

#define user_exists(x) \
  file_exists("/data/user/connection/"+ x[0..0] + "/" + x + ".o")

#define member_group(uid, group) GROUP_D->is_member(group, uid)

// This is used to resolve a path with respect to this_player()
#define RESOLVE_PATH(x) this_player()->path_resolve(x)

#endif
