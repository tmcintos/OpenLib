/*  -*- LPC -*-  */
/*    /secure/cmds/player/tell.c
 *    from the Nightmare IV LPC Library
 *    the tell command
 *    created by Descartes of Borg 950523
 */

#include <mudlib.h>
#include <net/daemons.h>
#include <command.h>

int main(string str) {
    string *words;
    mixed mud;
    object ob;
    int i, maxi;
    string who, msg, tmp;

    if(!str) return notify_fail("Syntax: <tell [who] [message]>\n");
    mud = 0; msg = 0;
#if 1
    if(sscanf(str, "%s@%s %s", who, tmp, msg) == 3 ||
       sscanf(str, "%s@%s", who, tmp) == 2) {
      if(!msg) return notify_fail("Syntax: <tell [who] [message]>\n");
      tmp = lower_case(replace_string(tmp, ".", " "));
      mud = (string)INTERMUD_D->GetMudName(tmp);
    }
#else
    if((maxi=sizeof(words = explode(str, "@"))) > 1) {
        who = convert_name(words[0]);
        if(maxi > 2) words[1] = implode(words[1..maxi-1], "@");
        maxi = sizeof(words = explode(words[1], " "));
        for(i=0; i<maxi; i++) {
	    string *mts;
            tmp = lower_case(implode(words[0..i], " "));
	    
	    if( (string)INTERMUD_D->GetMudName(tmp) ) {
                mud = tmp;
                if(i+1 < maxi) msg = implode(words[i+1..maxi-1], " ");
                else msg = "";
                break;
            }
        }
        if(!msg || msg == "")
	  return notify_fail("Syntax: <tell [who] [message]>\n");
        if(!mud) mud = -1;
      }
#endif
      if(!mud || mud == -1) {
        maxi = sizeof(words = explode(str, " "));
        who = 0;
        for(i=0; i<maxi; i++) {
            if(ob=find_living(tmp=convert_name(implode(words[0..i], " ")))) {
                who = tmp;
                if(i+1 < maxi) msg = implode(words[i+1..maxi-1], " ");
                else msg = "";
                break;
            }
        }
        if(!who) {
            if(!mud) return notify_fail("Tell whom what?\n");
            else return notify_fail(mud_name()+" is not aware of that mud.\n");
        }
        if(msg == "") return notify_fail("What do you wish to tell?\n");
    }
    else {
	SERVICES_D->eventSendTell(who, mud, msg);
	return 1;
    }
    if(ob) {
        string frm;

#if 0
        if(archp(ob)) frm = (string)this_player()->GetCapName();
        else frm = (string)this_player()->GetName();
        message("tell", frm + " tells you: "+msg, ob);
        if((int)ob->GetInvis(this_player()) || hiddenp(ob)) {
            ob->set_rproperty("reply", (string)this_player()->GetKeyName());
            message("error", "Tell whom what?", this_player());
            return 1;
        }
#else
//        if(archp(ob)) frm = (string)this_player()->GetCapName();
        frm = (string)this_player()->query_cap_name();
        message("tell", frm + " tells you: "+msg+"\n", ob);
        if((int)ob->GetInvis(this_player())) {
            ob->set_rproperty("reply", (string)this_player()->GetKeyName());
            message("error", "Tell whom what?\n", this_player());
            return 1;
        }
#endif
        else if(userp(ob) && !interactive(ob))
          message("self", (string)ob->GetCapName()+
            " is net-dead.\n", this_player());
        else if(userp(ob) && (query_idle(ob) > 60))
          message("self", (string)ob->GetName()+
            " is idle and may not have been paying attention.\n",
		  this_player());
        else if((int)ob->GetBlocked("tell")) { 
            message("self", (string)ob->GetName()+" is blocking "+
            "all tells.\n", this_player());
            return 1;
        }
        else if(in_edit(ob) || in_input(ob))
          message("self", (string)ob->GetCapName()+" is in input "+
            "and may not be able to respond.\n", this_player());
        message("self", "You tell "+(string)ob->query_cap_name()+
          ": "+msg+"\n", this_player());
        ob->SetProperty("reply", (string)this_player()->GetKeyName());
    }
    return 1;
}
  
void help() {
    message("help",
      "Syntax: <tell [player] [message]>\n"
      "        <tell [player]@[mud] [message]>\n\n"
      "Sends the message to the player named either on this mud if no "
      "mud is specified, or to the player named on another mud when "
      "another mud is specified.  For muds with more than one word in their "
      "names, use . (periods) to take place of spaces.  Example: tell "
      "descartes@realms.of.chaos hi\n\n"
      "See also: say, shout, yell, emote", this_player()
    );
}

string morse(string msg) {
mapping __Morse;
    string tmp;
    int x, i;
__Morse = ([ "a" : ".-", "b" : "-...", "c" : "-.-.",
"d" : "-..", "e" : ".", "f" : "..-.", "g" : "--.", "h" : "....", "i" : "..",
"j" : ".---", "k" : "-.-", "l" : ".-..", "m" : "--", "n" : "-.", "o" : "---", 
"p" : ".--.", "q" : "--.-", "r" : " .-.", "s" : "...", "t" : "-", "u" : "..-", 
"v" : "...-", "w" : ".--", "x" : "-..-", "y" : "-.--", "z" : "--..",
"1" : ".----", "2" : "..---", "3" : "...--", "4" : "....-", "5" : ".....",
"6" : " -....", "7" : "--...", "8" : "---..", "9" : "----.","0" : " -----" ]);
    for(tmp = "", x = strlen(msg), i=0; i< x; i++) {
        if(__Morse[msg[i..i]]) tmp += __Morse[msg[i..i]]+" ";
        else tmp += msg[i..i]+ " ";
    }
    return tmp;
}
