#include "std.h"

#undef EXTRA_RESET
#define EXTRA_RESET\
   set_items(([ "well" :\
                "You look down the well, but see only darkness.\n"\
                "There are some iron handles on the inside.\n"]));

#undef EXTRA_INIT
#define EXTRA_INIT add_action("go_down", "down");

THREE_EXIT("room/vill_road1","north",
	 "room/bank", "east",
	 "room/post", "south",
	 "Narrow alley",
	 "A narrow alley. There is a well in the middle.\n", 1)

go_down() {
    call_other(this_player(), "move_player", "down#room/well");
    return 1;
}
