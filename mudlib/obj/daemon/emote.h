// emote.h
// Daemon designed to handle emotes in every way.
// Lol   10/17/95


void emote_cmd(string str) {
  string this_emote, room_emote;
  string verb=query_verb();

string emote_file="/cmd/emotes/"+verb;
if(file_size(emote_file) > 0) {
  string this_emote=read_file(emote_file);
  tell_object(this_player(),"You "+verb+" "+this_emote);
  tell_room(environmet(this_player(),this_player()->query_cap_name()+" "s "+this_emote);
  return 1;
  }
return 0;
}
