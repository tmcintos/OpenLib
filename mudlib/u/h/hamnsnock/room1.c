#include <mudlib.h>

inherit ROOM;

string east_door_open;

reset(arg){
  if(arg)
    return;
  set_light(1);
  east_door_open = 0;
}

init(){
   add_action("open_door" , "open");
   add_action("close_door" , "close");
   add_action("go_east" , "east");
}

long(){
    if(east_door_open)
       write("You stand in a large test room.  There are windows everywhere on all four walls.  There is a door to the east of you, which is open.\n");
    if(!east_door_open)
       write("You stand in a large test room.  There are windows everywhere on all four walls.  There is a door to the east of you, which is closed.\n");
}

open_door(string str)
{
   if(!str){
       write("Open what door?\n");
}
    if(east_door_open)
         write("The door is already open.\n");
    if(!east_door_open){
         east_door_open = 1;
         write("You open the east door.\n");
    }
}

close_door(){
    if(east_door_open){
           east_door_open = 0;
           write("You close the east door.\n");
         return 1;
    }
    if(!east_door_open){
         write("The door is already closed!\n");
    }
}

go_east(){
    if(!east_door_open)
       write("The door seems to be closed.\n");
    if(east_door_open){
    this_player()->move("/d/base/start.c");
    this_player()->force_me("look");
       east_door_open = 0;
    }
}
