/*    /lib/user/nmsh.c
 *    from Foundation II
 *    the new Nightmare shell
 *    created by Descartes of Borg 940216
 */

#include <config.h> 
#include <flags.h>
#include "nmsh.h"
 
#define DIRECTORY_STACK_SIZE     5
#define MAX_CMD_ALIASES          60
#define MIN_HISTORY_SIZE         10

private string CurrentWorkingDirectory;
private mapping Nicknames, Aliases, Xverbs; 
private static int CWDCount, CWDBottom, CWDTop, CmdNumber; 
private static int HistoryCount, HistoryBottom, HistoryTop, HistorySize; 
private static string Prompt; 
private static string *Stack, *History; 
 
static void create() { 
    Nicknames = ([]); 
    Aliases = ([ "l" : "look $*", "bio" : "biography", "i" : "inventory",
      "n" : "go north", "s" : "go south", "e" : "go east", "w" : "go west",
      "ne" : "go northeast", "nw" : "go northwest", "se" : "go southeast",
      "sw" : "go southwest", "d" : "go down", "u" : "go up", 
      "exa" : "look at $*",
    ]);
    Xverbs = ([ "'" : "say $*", ":" : "emote $*" ]); 
    CurrentWorkingDirectory = "/doc";
} 
 
int Setup() { 
    if(this_player() != this_object()) return 0; 
    reset_history(); 
    reset_prompt(); 
    if(!Nicknames) Nicknames = ([]);
    if(!Aliases) Aliases = ([]);
    if(!Xverbs) Xverbs = ([]);
    add_action("cmd_alias", "alias"); 
    add_action("cmd_history", "history");
    add_action("cmd_nickname", "nickname"); 
    if(creatorp(this_object())) { 
        Stack = allocate(DIRECTORY_STACK_SIZE); 
        CWDBottom = CWDTop = CWDCount = 0; 
        add_action("cmd_cd", "cd"); 
        add_action("cmd_nmsh", "nmsh"); 
        add_action("cmd_pushd", "pushd"); 
        add_action("cmd_popd", "popd"); 
        add_action("cmd_pwd", "cwd");
        add_action("cmd_pwd", "pwd");
        add_action("cmd_work", "work");
    } 
    return 1;
} 
 
nomask static int cmd_alias(string str) { 
    string *a, *b;
    string key, thing; 
    int i;
 
    if(this_player() != this_object()) return 0; 
    if(!str) {
        i = sizeof(a = keys(Aliases));
        while(i--)
          a[i] = sprintf("%s%s", arrange_string(a[i],15),Aliases[a[i]]);
        i = sizeof(b = keys(Xverbs));
        while(i--)
          b[i] = sprintf("$%s%s", arrange_string(b[i],14), Xverbs[b[i]]);
        this_player()->more(a+b);
        return 1;
    }
    if(sscanf(str, "%s %s", key, thing) != 2) { 
        if(str[0] == '$') { 
            str = str[1..strlen(str)-1]; 
            if(Xverbs[str]) { 
                map_delete(Xverbs, str); 
                message("system", sprintf("Alias $%s removed.", str), this_player()); 
            } 
            else message("system", sprintf("No such alias $%s.", str), this_player()); 
            return 1; 
        } 
        if(Aliases[str]) { 
            map_delete(Aliases, str); 
            message("system", sprintf("Alias %s removed.", str), this_player()); 
        } 
        else message("system", sprintf("No such alias %s.", str), this_player()); 
        return 1; 
    } 
    if(sizeof(Xverbs) + sizeof(Aliases) >= MAX_CMD_ALIASES) { 
        message("system", "You must remove an alias before adding another.", 
          this_player()); 
        return 1; 
    } 
    if(key == "alias") return notify_fail("You are a bonehead.\n");
    if(key[0] == '$') { 
        key = key[1..strlen(key)]; 
        if(Xverbs[key])  
          message("system", sprintf("Alias for $%s altered to (%s).", 
            key, thing), this_player()); 
        else message("system", sprintf("Alias $%s (%s) added.", key, thing), 
          this_player());      
        Xverbs[key] = thing; 
    } 
    else { 
        if(Aliases[key]) 
          message("system", sprintf("Alias for %s altered to (%s).", key, thing), 
            this_player()); 
        else message("system", sprintf("Alias %s (%s) added.", key, thing),this_player()); 
        Aliases[key] = thing; 
    } 
    return 1; 
} 
 
nomask static int cmd_cd(string str) { 
    if(this_player() != this_object()) return 0; 
    set_cwd(str); 
    return 1; 
} 
 
nomask static int cmd_history(string str) {
    string *cmds;
    int i, maxi, x;

    if(CmdNumber <= HistorySize) {
        for(i=0; i<CmdNumber; i++) 
          message("system", sprintf("%d%s",i+1,
            ((i+1) > 9 ? " "+History[i] : "  "+History[i])),
            this_player());
        return 1;
    }
    maxi = sizeof(cmds = History[HistoryBottom..HistorySize-1] +
      History[0..HistoryTop]);
    for(i=0; i<maxi; i++) {
        x = CmdNumber-HistorySize+i+1;
        message("system", sprintf("%d %s", x, cmds[i]), this_player());
    }
    return 1;
}

nomask static int cmd_nickname(string str) { 
    string *cles;
    string key, thing; 
    int i;
 
    if(this_player() != this_object()) return 0;
    if(!str) {
        i = sizeof(cles = keys(Nicknames));
        while(i--)
          cles[i] = sprintf("%s%s", arrange_string(cles[i], 15),
            Nicknames[cles[i]]);
        this_player()->more(cles);
        return 1;
    }
    if(sscanf(str, "%s %s", key, thing) != 2) { 
        if(Nicknames[str]) { 
            message("system", sprintf("Nickname %s removed.", str), 
              this_player()); 
            map_delete(Nicknames, str); 
        } 
        else message("system", sprintf("No such nickname %s.", str), 
          this_player()); 
    } 
    else { 
        if(Nicknames[key])  
          message("system", sprintf("Nickname %s altered to (%s).", key, thing), 
            this_player()); 
        else message("system", sprintf("Nickname %s (%s) added.", key, thing), 
          this_player()); 
        Nicknames[key] = thing; 
    } 
    return 1; 
} 
 
nomask static int cmd_nmsh(string str) { 
    string *lines; 
    string tmp;
    int i, maxi; 
 
    if(!str) return 0; 
    if(this_player() != this_object()) return 0; 
    if((int)this_player()->GetForced()) return 0; 
    if(!(tmp = read_file(absolute_path(query_cwd(), str)))) 
      return notify_fail(sprintf("nmsh: script %s not found.\n")); 
    maxi = sizeof(lines = explode(tmp, "\n")); 
    for(i=0; i < maxi; i++) { 
        if(lines[i][0] == '#') continue; 
        if(!command(lines[i])) { 
            message("system", sprintf("nmsh: error in executing %s.", str), 
              this_player()); 
            return 1; 
        } 
    } 
    return 1; 
} 
 
nomask static int cmd_pushd(string str) { 
    if(this_player() != this_object()) return 0; 
    if(!set_cwd(str)) return 0; 
    pushd(str); 
    return 1; 
} 
 
nomask static int cmd_popd(string str) { 
    if(this_player() != this_object()) return 0; 
    set_cwd(popd()); 
    return 1; 
} 
 
nomask static int cmd_pwd(string str) {
    if(!query_cwd()) message("system", "No current directory.", this_object());
    else message("system", query_cwd()+":", this_object());
    return 1;
}

nomask static int cmd_work(string str) {
    string *tmp;
    object ob;
    string file;
    int flag;

    if(!str || str == "") ob = environment(this_object());
    else if(str == "!") {
        flag = 1;
        ob = environment(this_object());
    }
    else if(str[0] == '!') {
        flag = 1;
        str = str[1..strlen(str)];
    }
    if(!ob && !(ob = present(str, this_object())))
      ob = present(str, environment(this_object()));
    if(!ob) return notify_fail("No target object found.\n");
    tmp = explode(file = base_name(ob), "/");
    set_cwd("/"+implode(tmp[0..sizeof(tmp)-2], "/"));
    if(flag) {
        message("system", file+".c, "+file_size(file+".c")+" bytes:",
          this_object());
        this_object()->eventEdit(file+".c");
    }
    return 1;
}

nomask string write_prompt() { 
    string tmp, ret; 
    int x, y;
 
    if( (y = query_ed_mode()) != -1 ) {
	if( !y ) {
	    if( creatorp() ) ret = ":";
	    else 
	      ret = "\tQ)uit without saving, save and ex)it, h)elp\nCommand: ";
	}
	else if( y == -2 ) ret = "Help: ";
	else ret = "*\b";
	message("prompt", ret, this_object());
	return ret;
    }
    if((ret = Prompt) == DEFAULT_PROMPT) {
        message("prompt", ret, this_object());
        return ret;
    }
    while((x = strsrch(ret, "$")) != -1) {
        if(x == strlen(ret) -1) break;
        switch(ret[x+1]) {
            case 'D': 
                if(!creatorp(this_object())) break;
                if(sscanf(query_cwd(), user_path(GetKeyName())+"%s",
                  tmp)) tmp = "~"+tmp;
                else tmp = query_cwd();
                ret = replace_string(ret, "$D", tmp); 
                break;
            case 'V': case 'v':
                if(GetInvis()) {
                    ret = replace_string(ret, "$V", "INVIS"); 
                    ret = replace_string(ret, "$v", "invis"); 
                } 
                else if(hiddenp(this_object())) { 
                    ret = replace_string(ret, "$V", "HID"); 
                    ret = replace_string(ret, "$v", "hid"); 
                } 
                else { 
                    ret = replace_string(ret, "$V", ""); 
                    ret = replace_string(ret, "$v", ""); 
                } 
                break;
            case 'C':
                ret = replace_string(ret, "$C", sprintf("%d", CmdNumber+1)); 
                break;
            default:
                ret = replace_string(ret, ret[x..x+1], "");
                break;
        }
    }
    message("prompt", ret, this_object());
    return ret;
} 
 
string process_input(string str) { 
    string tmp, xtra, request; 
 
    if(!str || str == "") return ""; 
    else if(GetClient() &&
      member_array(GetClient(), SUPPORTED_CLIENTS) != -1) {
        if(sscanf(str, "<%s>%s", request, xtra)) {
            process_request(request, xtra ? xtra : "");
            return "";
        }
        else return str;
    }
    else if((tmp = do_history(str)) == "") return ""; 
    if(tmp != str) message("system", tmp, this_object());
    add_history_cmd(tmp); 
    return cache_commands(do_alias(do_nickname(tmp)));
} 
 
nomask static void process_request(string request, string xtra) {
    switch(request) {
        case "ALIAS":
            receive("<ALIAS>[n,go north] [s,go south] [e,go east] [w,go west] "
              "[nw,go northwest] [ne,go northeast] [sw,go southwest] "
              "[se,go southeast] [u,go up] [d,go down] [i,inventory] "
              "[bio,biography] [exa,look at $*] [$',say $*] "
              "[$:,emote $*] [l,look]\n");
            break;
        case "NICKNAME": receive("<NICKNAME>\n"); break;
        case "USERS":
            receive("<USERS>"+implode(map_array(filter(users(), 
              "request_vis", this_object()), "user_names", this_object()), 
              ", ")+"\n");
            break;
        case "ROOM":
            receive("<ROOM>"+
              (string)environment(this_object())->GetShort()+"\n");
            break;
	case "PRESENT":
            receive("<PRESENT>"+
              implode(map_array(filter(all_inventory(environment(this_object())),
              "request_vis", this_object()), "user_names", this_object()),
              ", ")+"\n");
            break;
        default:
            receive("<error>Request not supported.\n");
            break;
	}
}

static int request_vis(object ob) {
    return (userp(ob) && !((int)ob->GetInvis(this_object())));
  }

static string user_names(object ob) {
    return (string)ob->GetName();
  }

private static int set_cwd(string str) { 
    int x;

    if(!str || str == "") str = user_path(GetKeyName()); 
    if (str[<1] == '/' && str != "/") str = str[0..<2];
    replace_string(str, "//", "/"); 
    str = absolute_path(query_cwd(), str);
    if((x=file_size(str)) != -2) { 
        if(x > -1) { 
            message("system", sprintf("%s: Path is a file.", str), this_player()); 
            return 0; 
        } 
        else { 
            message("system", sprintf("%s: No such reference.", str), this_player()); 
            return 0; 
        }  
    } 
    CurrentWorkingDirectory = str; 
    message("system", sprintf("%s:", CurrentWorkingDirectory), this_player()); 
    return 1; 
} 
 
private static void pushd(string str) { 
    if(CWDCount++ == DIRECTORY_STACK_SIZE) { 
        CWDCount--; 
        CWDBottom = (++CWDBottom) % DIRECTORY_STACK_SIZE; 
    } 
    Stack[CWDTop] = str; 
    CWDTop = (++CWDTop) % DIRECTORY_STACK_SIZE; 
} 
 
private static string popd() { 
    if(!CWDCount) return 0; 
    CWDCount--; 
    return Stack[--CWDTop]; 
} 
 
nomask private static string do_nickname(string str) { 
    if(!Nicknames) return str; 
    if(str[0..7] == "nickname") return str; 
    return implode(map_array(explode(str, " "), "replace_nickname", this_object()), " "); 
} 
 
nomask private static string do_alias(string str) { 
    string *words; 
    string tmp; 
    int x, i; 
 
    if(!sizeof(words = explode(str, " "))) return "";
    if((x = strlen(words[0])) && (tmp = Xverbs[words[0][0..0]])) {
        words[0] = words[0][1..x-1];
        return replace_string(tmp, "$*", implode(words, " "));
    }
    if(!(tmp = Aliases[words[0]])) return implode(words, " "); 
    else str = implode(words[1..sizeof(words)-1], " "); 
    return replace_string(tmp, "$*", str); 
 
} 
 
nomask private static string do_history(string str) { 
    string *args;
    string tmp, cmd, expr;
    int x, slen, i; 
 
    if(str[0] != '!') return str; 
    if(str == "!!" || str == "!-1") {
        if(CmdNumber == 0) { 
            message("system", "Try entering a command first.", this_player()); 
            return ""; 
        } 
        else return History[(CmdNumber-1) % HistorySize]; 
    } 
    if(sscanf(str, "%ss/%s", cmd, expr) == 2) { 
        if((tmp = do_history(cmd)) == "") return ""; 
        switch(sizeof(args = explode(expr, "/"))) { 
        case 2: 
            if((x = strsrch(tmp, args[0])) == -1) { 
                message("system", "nmsh: History expression not found.", 
                  this_player()); 
                return ""; 
            } 
            else if(!x)
              return sprintf("%s%s",args[1],tmp[strlen(args[0])..strlen(tmp)-1]);
            else return sprintf("%s%s%s", tmp[0..x-1], args[1], 
              tmp[x+strlen(args[0])..strlen(tmp)-1]); 
        case 3: 
            if(args[2] != "g") { 
                message("system", "nmsh: History syntax error.", 
                  this_player()); 
                return ""; 
            } 
            else return replace_string(tmp, args[0], args[1]); 
        default: 
            message("system", "nmsh: History syntax error.", this_player()); 
            return ""; 
        } 
    } 
    if(sscanf(str, "!%d", x) == 1) { 
        if(x < 1) x = 1+ CmdNumber + x;
        if(x < 1 || x >= 1 + CmdNumber) { 
            message("system", sprintf("Event %d not found.", x), 
              this_player()); 
            return ""; 
        } 
        if(CmdNumber - HistorySize > x) { 
            message("system", sprintf("Event %d is lost from the queue.", x), 
              this_object()); 
            return ""; 
        } 
        else return History[(x-1) % HistorySize]; 
    } 
    else { 
        if(CmdNumber <= HistorySize)
          i=sizeof(args=map_array(History[0..CmdNumber-1],"replace_null",
            this_object()));
        else i=sizeof(args=map_array(History[HistoryBottom..HistorySize-1]+
          History[0..HistoryTop], "replace_null", this_object()));
        while(i--) if(!strsrch(args[i], str[1..strlen(str)-1])) return args[i];
        return "";
    } 
} 
 
nomask static string replace_null(string str) { 
    if(!str) return ""; 
    else return str; 
} 
         
nomask private static void add_history_cmd(string str) { 
    History[HistoryTop = (++CmdNumber-1) % HistorySize] = str;
    if(CmdNumber < HistorySize) HistoryBottom = 0;
    else if(!HistoryTop) HistoryBottom = HistorySize - 1;
    else HistoryBottom = HistoryTop + 1;
}
             
nomask static string replace_nickname(string str) { 
    if(str == "") return str; 
    if(str[0] == '\\') return str[1..(strlen(str)-1)]; 
    else if(Nicknames[str]) return Nicknames[str]; 
    else return str; 
} 
 
void reset_history() { 
    CmdNumber = 0; 
    HistoryCount = HistoryBottom = HistoryTop = 0; 
    History = allocate(HistorySize = query_history_size()); 
} 
 
void reset_prompt() { 
    Prompt =GetPrompt(); 
    if(!stringp(Prompt)) Prompt = "Prompt screwey> ";
    Prompt =replace_string(Prompt, "$M", mud_name()); 
    Prompt =replace_string(Prompt, "$m", lower_case(mud_name())); 
    Prompt =replace_string(Prompt, "$N", (string)this_object()->GetCapName()); 
    Prompt =replace_string(Prompt, "$n", GetKeyName());
} 
 
string query_cwd() { return CurrentWorkingDirectory; } 
 
int query_history_size() { return MIN_HISTORY_SIZE; } 
     
string GetPrompt() { return DEFAULT_PROMPT; }

string get_path() { return query_cwd(); }

varargs int GetInvis(object ob) { return 0; }

string GetKeyName() { return 0; }

string GetCapName() { return 0; }

static string cache_commands(string str) {
    function f;
    object ob;
    string cmd, arg;
    int x;

    x = strsrch(str, " ");
    if( x == -1 ) {
	cmd = str;
	arg = 0;
    }
    else if(!x) {
	if(strlen(str) == 1) return "";
        else return cache_commands(trim(str));
    }
    else {
	cmd = str[0..x-1];
	if(x == strlen(str) -1) arg = 0;
	else arg = str[x+1..];
    }
    switch(cmd) {
	case "passwd":
	  if( sizeof(previous_object(-1)) ) return "";
	  message("system", "Changing password for " + GetCapName() +
		  " on " + mud_name() + ".", this_object());
	  message("prompt", "Old password: ", this_object());
	  f = function(string arg) {
	      function g;
	      string oldpass;

	      if( arg == "" || !arg ) {
		  message("system", "\nPassword change failed.", 
			  this_object());
		  return;
	      }
	      oldpass = (string)this_object()->GetPassword();
	      if( oldpass != crypt(arg, oldpass) ) {
		  message("system", "\nPassword change failed.", 
			  this_object());
		  return;
	      }
	      message("prompt", "\nNew password: ", this_object());
	      g = function(string arg) {
		  function h;

		  if( !arg || strlen(arg) < 5 ) {
		      message("system", "\nPasswords must be 5 characters "
			      "in length.", this_object());
		      return;
		  }
		  message("prompt", "\nConfirm password: ", this_object());
		  h = function(string arg, string chkpass) {
		      if( arg != chkpass ) {
			  message("system", "\nPasswords do not match.",
				  this_object());
			  return;
		      }
		      if( this_player(1) != this_object() ) return;
		      this_object()->SetPassword(crypt(arg, 0));
		      message("system", "\nPassword changed.", this_object());
		  };
		  input_to(h, I_NOECHO | I_NOESC, arg);
	      };
	      input_to(g, I_NOECHO | I_NOESC);
	  };
	  input_to(f, I_NOECHO | I_NOESC);
	  return "";
        case "go":
	  if( !(ob = environment()) ) return str;
	  else if( (int)ob->cmdGo(arg) ) return "";
	  else return str;
        case "enter":
	  if( !(ob = environment()) ) return str;
	  else if( (int)ob->cmdEnter(arg) ) return "";
	  else return str;
        case "look":
	  if( !ob = environment() ) {
	      message("room_description", "You are nowhere.", this_object());
	      return "";
	  }
	  else if( (int)ob->cmdLook(arg) ) return "";
	  else return str;
        default: return str;
    }
}  
