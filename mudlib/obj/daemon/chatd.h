class channel_info {
  string desc;             /* description of channel */
  int min_lvl;             /* minimum level to chat, -1 == no-one 0 == unltd */
  string guild;            /* required guild to chat here */
  object *userlist;        /* list of subscribed users */
}

#define chan_i class channel_info

int chat(string str);
string map_to_local(string ch);
string map_to_remote(string ch);
varargs void broadcast_local(string ch, string who, string msg, int emote,
			     string target, string targetmsg);
