inherit "room/room";

reset(arg) {
    if (arg) return;
    dest_dir = ({ "room/vill_road1","west",
		  "room/vill_shore","east",
		  "room/adv_guild","south",
		  "room/station", "down",
		  "room/shop","north" });
    short_desc = "Village road";
    no_castle_flag = 1;
    long_desc = "A long road going through the village. There are stairs going down.\n" +
	"The road continues to the west. To the north is the shop, and to the\n" +
        "south is the adventurers guild. The road runs towards the shore to\n"+
        "the east.\n";
    set_light(1);
}

down() {
    call_other(this_player(), "move_player", "down#room/station");
    return 1;

}
