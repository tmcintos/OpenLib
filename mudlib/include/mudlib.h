/*  -*- LPC -*-  */
// file: mudlib.h
#ifndef _MUDLIB_H
#define _MUDLIB_H

#include <dirs.h>

// Objects
#define CONNECTION_OB CLONE_DIR "/connection"
#define USER_OB      CLONE_DIR "/user"             // default user body
#define MASTER_OB    SECURE_DAEMON_DIR "/master"
#define VOID_OB      SECURE_DAEMON_DIR "/void"

#define BASE         INHERIT_DIR "/object"    // ! take this out soon !
#define OBJECT       INHERIT_DIR "/object"
#define CONTAINER    INHERIT_DIR "/container"
#define ROOM	     INHERIT_DIR "/room"
#define LIVING       INHERIT_DIR "/living"
#define DAEMON       INHERIT_DIR "/daemon"
#define SHOP         INHERIT_DIR "/shop"
#define WEAPON       INHERIT_DIR "/weapon"
#define ARMOUR       INHERIT_DIR "/armour"
#define USER         CLONE_DIR "/user"
#define MONSTER      CLONE_DIR "/monster"

#define SIMUL_EFUN   SECURE_DIR "/simul_efun"

#define CLEAN_UP     INHERIT_DIR "/clean_up"

#endif /* _MUDLIB_H */
