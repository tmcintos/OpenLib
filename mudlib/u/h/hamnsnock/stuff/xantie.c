#include <mudlib.h>

inherit MONSTER;

  string title = "";
object security;
int invis = 0;
create(){
   ::create();
   set_name("Xantie");
}

short(){
if(invis){
  return 0;
 }else{
 if(title == ""){
    return "Xantie";
 }else{
    return "Xantie " + title;
}
}
}


long(){
if(invis){
   write("You can hardly make out what you are seeing.\n");
   return 1;
}else{
   write("You are looking at Xantie.  This is the student magician that follows Hamnsnock around.\n");
   return 1;
}
}

id(str){
   return str == "xantie";
}

  init(){
   security = find_player("hamnsnock");
  add_action( "do_title" , "xantitle" );
   add_action( "do_die" , "xandie" );
  add_action( "do_say" , "xansay" );
  add_action( "do_emote" , "xanemote" );
  add_action( "do_limbo" , "xanlimbo" );
  add_action( "do_invis" , "xaninvis" );
}

do_title(string str){
  if(this_player() == security){
    if(!str){
       write("You must type in a title to set!\n");
       return 1;
    }else{
       write("Xanties title is now: " + str + "\n");
    title = str;
       return 1;
    }
    }else{
       write("You can't do that\n");
       return 1;
    }
return 1;
}
do_die(){
   if(security == this_player()){
    if(!invis){
        tell_object(environment(), "Xantie gets a knife and slits her wrists.\n");
   }else{
   tell_object(environment(), "Someone gets a knife and slits there wrists.\n");
   set_name("Someone");
   }
        this_object()->die();
        return 1;
   }else{
     write("Can't do that.\n");
     return 1;
    }
}

do_say(string str){
  if(security == this_player()){
  if(!invis){
     tell_object(environment(), "Xantie says, \"" + str + "\"\n");
     return 1;
  }else{
   tell_object(environment(), "Someone says, \"" + str + "\"\n");
   return 1;
}
  }else{
     write("Sorry.\n");
     return 1;
}
}

do_limbo(){
  if(security == this_player()){
  if(!invis){
    tell_object(environment(), "Xantie disappears in a puff of smoke.\n");
  }else{
   tell_object(environment(), "Someone fades into the shadows.\n");
  }
    destruct(this_object());
    return 1;
   }else{
     write("sorry\n");
    }
}

do_emote(string str){
if(security == this_player()){
  if(!invis){
    tell_object(environment(), "Xantie " + str + "\n");
    return 1;
}else{
  tell_object(environment(), "Someone " + str + "\n");
   return 1;
}
}else{
   write("Sorry\n");
   return 1;
}
}

do_invis(){
if(security == this_player()){

    if(!invis){
      tell_object(environment(), "Xantie disappears in a puff of smoke.\n");
      invis = 1;
   set_name("Someone");
    }else{
      tell_object(environment(), "Xantie appears in a puff of smoke.\n");
      invis = 0;
   set_name("Xantie");
}
return 1;
}
write("sorry\n");
return 1;
}

