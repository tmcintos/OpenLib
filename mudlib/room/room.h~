#include <mudlib.h>

inherit ROOM;

/* An array with destinations and directions: "room/church", "north" ... */
string dest_dir;

/* Short description of the room */
string short_desc;

/* Long description of the room */
string long_desc;

/* Special items in the room. "table", "A nice table", "window", "A window" */
string* items;

/* Fact about this room. ex: "no_fight", "no_steal" */
string property;

/* No castles are allowed to be dropped here */
int no_castle_flag;

#define EXTRA_RESET
#define EXTRA_INIT
#define EXTRA_LONG

#define ONE_EXIT(DEST, DIR, SH, LO, LIGHT)\
void reset(int arg) { EXTRA_RESET }\
void create() {\
  ::create();\
  reset(0);\
  set_light(LIGHT); set_short(SH); set_long(LO);\
  set_exits(([ DIR : "/"DEST ])); } \
void init() { ::init(); EXTRA_INIT }\
varargs mixed long(int flag) { EXTRA_LONG ::long(flag); }

#define TWO_EXIT(DEST1, DIR1, DEST2, DIR2, SH, LO, LIGHT)\
void reset(int arg) { EXTRA_RESET }\
void create() {\
  ::create();\
  reset(0);\
  set_light(LIGHT); set_short(SH); set_long(LO);\
  set_exits(([ DIR1 : "/"DEST1, DIR2 : "/"DEST2 ])); } \
void init() { ::init(); EXTRA_INIT }\
varargs mixed long(int flag) { EXTRA_LONG ::long(flag); }

#define THREE_EXIT(DEST1, DIR1, DEST2, DIR2, DEST3, DIR3, SH, LO, LIGHT)\
void reset(int arg) { EXTRA_RESET }\
void create() {\
  ::create();\
  reset(0);\
  set_light(LIGHT); set_short(SH); set_long(LO);\
  set_exits(([ DIR1 : "/"DEST1, DIR2 : "/"DEST2, DIR3 : "/"DEST3 ])); } \
void init() { ::init(); EXTRA_INIT }\
varargs mixed long(int flag) { EXTRA_LONG ::long(flag); }

#define FOUR_EXIT(DEST1, DIR1, DEST2, DIR2, DEST3, DIR3, DEST4, DIR4, SH, LO, LIGHT)\
void reset(int arg) { EXTRA_RESET }\
void create() {\
  ::create();\
  reset(0);\
  set_light(LIGHT); set_short(SH); set_long(LO);\
  set_exits(([ DIR1 : "/"DEST1, DIR2 : "/"DEST2, DIR3 : "/"DEST3, DIR4 : "/"DEST4 ])); } \
void init() { ::init(); EXTRA_INIT }\
varargs mixed long(int flag) { EXTRA_LONG ::long(flag); }

#define move_object(x, y) x->move(y)
