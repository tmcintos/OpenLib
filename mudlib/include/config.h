// config.h:  global include file
//
//   This file is _automatically_ #included in _all_ objects...
//   So you don't have to include it!
//
#ifndef _CONFIG_H
#define _CONFIG_H

#include <runtime_config.h>

#define TRUE  1
#define FALSE 0

#define status int          // for backwards compat
#define char int            // for read-ability
#define boolean int         // for read-ability

#define mud_name() __MUD_NAME__
#define mudlib() "UltraLib"
#define mudlib_version() "0.1"
#define version() __VERSION__

// Crap for quick n dirty compatibility with the Intermud3 stuff

#define convert_name(x) lower_case(replace_string(x, " ", ""))
#define identify(x) typeof(x)

// (Tim) put in to make it easier to change later...

#define RESOLVE_PATH(x) this_player()->path_resolve(x)

#endif
