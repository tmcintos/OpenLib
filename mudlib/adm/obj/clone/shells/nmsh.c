/*    /lib/user/nmsh.c
 *    from Foundation II
 *    the new Nightmare shell
 *    created by Descartes of Borg 940216
 *
 * NOTE: This file is NOT subject to the terms of the OpenLib License agreement
 *
 *    2.29.96  Tim ported over to UltraLib.
 *             Left 3 defines in that may be useful.  Also calls some functions
 *             not presently in the user like query_invis() and query_client()
 */

#include <daemons.h>
#include <shells.h>
#include "nmsh.h"

#define hiddenp(ob) 0
#define SUPPORTED_CLIENTS ({})
#define DEFAULT_PROMPT "nmsh% "
 
#define DIRECTORY_STACK_SIZE     5
#define MAX_CMD_ALIASES          60
#define MIN_HISTORY_SIZE         10

private string CurrentWorkingDirectory;
private mapping Nicknames, Aliases, Xverbs, Env; 
private static int CWDCount, CWDBottom, CWDTop, CmdNumber; 
private static int HistoryCount, HistoryBottom, HistoryTop, HistorySize; 
private static string Prompt; 
private static string *Stack, *History; 
private static object owner;
 
object shell_init(object ob) { 
    if(owner) return;

    Nicknames = ([]); 
    Aliases = ([ "l" : "look $*", "bio" : "biography", "i" : "inventory",
      "n" : "north", "s" : "south", "e" : "east", "w" : "west",
      "ne" : "northeast", "nw" : "northwest", "se" : "southeast",
      "sw" : "southwest", "d" : "down", "u" : "up", 
      "exa" : "look at $*",
    ]);
    Xverbs = ([ "'" : "say $*", ":" : "emote $*" ]); 

    if(!(owner = shadow(ob))) return 0;

    reset_history();
    reset_prompt();

    Env = (mapping) owner->get_environment();

    if(!Env["nmsh_nick"]) Env["nmsh_nick"] = Nicknames;
    else Nicknames = Env["nmsh_nick"];

    if(!Env["nmsh_alias"]) Env["nmsh_alias"] = Aliases;
    else Aliases = Env["nmsh_alias"];

    if(!Env["nmsh_xverb"]) Env["nmsh_xverb"] = Xverbs;
    else Xverbs = Env["nmsh_xverb"];

    if(wizardp(owner)) { 
      CurrentWorkingDirectory = user_cwd(owner->query_name());
      Stack = allocate(DIRECTORY_STACK_SIZE); 
      CWDBottom = CWDTop = CWDCount = 0; 
    } 
    return owner;
} 
 
nomask static int cmd_alias(string str) { 
    string *a, *b;
    string key, thing; 
    int i;
 
    if(!str) {
        i = sizeof(a = keys(Aliases));
        while(i--)
          a[i] = sprintf("%-10s  %-s", a[i], Aliases[a[i]]);
        i = sizeof(b = keys(Xverbs));
        while(i--)
          b[i] = sprintf("$%-10s  %-s", b[i], Xverbs[b[i]]);
        owner->more(a+b);
        return 1;
    }
    if(sscanf(str, "%s %s", key, thing) != 2) { 
        if(str[0] == '$') { 
            str = str[1..strlen(str)-1]; 
            if(Xverbs[str]) { 
                map_delete(Xverbs, str); 
                message("system", sprintf("Alias $%s removed.\n", str), owner); 
            } 
            else message("system", sprintf("No such alias $%s.\n", str), owner); 
            return 1; 
        } 
        if(Aliases[str]) { 
            map_delete(Aliases, str); 
            message("system", sprintf("Alias %s removed.\n", str), owner); 
        } 
        else message("system", sprintf("No such alias %s.\n", str), owner); 
        return 1; 
    } 
    if(sizeof(Xverbs) + sizeof(Aliases) >= MAX_CMD_ALIASES) { 
        message("system", "You must remove an alias before adding another.\n", 
          owner); 
        return 1; 
    } 
    if(key == "alias") return notify_fail("You are a bonehead.\n");
    if(key[0] == '$') { 
        key = key[1..strlen(key)]; 
        if(Xverbs[key])  
          message("system", sprintf("Alias for $%s altered to (%s).\n", 
            key, thing), owner); 
        else message("system", sprintf("Alias $%s (%s) added.\n", key, thing), 
          owner);      
        Xverbs[key] = thing; 
    } 
    else { 
        if(Aliases[key]) 
          message("system", sprintf("Alias for %s altered to (%s).\n", key, thing), 
            owner); 
        else message("system", sprintf("Alias %s (%s) added.\n", key, thing),owner); 
        Aliases[key] = thing; 
    } 
    return 1; 
} 

private 
nomask static int cmd_cd(string str) { 
    set_cwd(str); 
    return 1; 
} 

nomask static int cmd_history(string str) {
    string *cmds;
    int i, maxi, x;

    if(CmdNumber <= HistorySize) {
        for(i=0; i<CmdNumber; i++) 
          message("system", sprintf("%d%s",i+1,
            ((i+1) > 9 ? " "+History[i] : "  "+History[i] +"\n")),
            owner);
        return 1;
    }
    maxi = sizeof(cmds = History[HistoryBottom..HistorySize-1] +
      History[0..HistoryTop]);
    for(i=0; i<maxi; i++) {
        x = CmdNumber-HistorySize+i+1;
        message("system", sprintf("%d %s\n", x, cmds[i]), owner);
    }
    return 1;
}

nomask static int cmd_nickname(string str) { 
    string *cles;
    string key, thing; 
    int i;
 
    if(!str) {
        i = sizeof(cles = keys(Nicknames));
        while(i--)
          cles[i] = sprintf("%-10s  %-s", cles[i], Nicknames[cles[i]]);
        owner->more(cles);
        return 1;
    }
    if(sscanf(str, "%s %s", key, thing) != 2) { 
        if(Nicknames[str]) { 
            message("system", sprintf("Nickname %s removed.\n", str), 
              owner); 
            map_delete(Nicknames, str); 
        } 
        else message("system", sprintf("No such nickname %s.\n", str), 
          owner); 
    } 
    else { 
        if(Nicknames[key])  
          message("system", sprintf("Nickname %s altered to (%s).\n", key, thing), 
            owner); 
        else message("system", sprintf("Nickname %s (%s) added.\n", key, thing), 
          owner); 
        Nicknames[key] = thing; 
    } 
    return 1; 
} 
 
nomask int cmd_prompt(string str) {
  if(!str || str == "") return reset_prompt(), 1;
  Prompt = replace_string(str, "\"", "");
  return 1;
}

nomask static int cmd_nmsh(string str) { 
    string *lines; 
    string tmp;
    int i, maxi; 
 
    if(!str) return notify_fail("usage: nmsh <script>\n"),0; 
    if((int)owner->GetForced()) return 0; 
    if(!(tmp = read_file(absolute_path(query_cwd(), str)))) 
      return notify_fail(sprintf("nmsh: script %s not found.\n", str)); 
    maxi = sizeof(lines = explode(tmp, "\n")); 
    for(i=0; i < maxi; i++) { 
        if(lines[i][0] == '#') continue; 
        if(!owner->force_me(lines[i])) { 
            message("system", sprintf("nmsh: error in executing %s.\n", str), 
              owner); 
            return 1; 
        } 
    } 
    return 1; 
} 
 
nomask static int cmd_pushd(string str) { 
    if(!set_cwd(str)) return 0; 
    pushd(str); 
    return 1; 
} 
 
nomask static int cmd_popd(string str) { 
    set_cwd(popd()); 
    return 1; 
} 
 
nomask static int cmd_pwd(string str) {
    if(!query_cwd())
      message("system", "No current directory.\n", owner);
    else message("system", query_cwd()+":\n", owner);
    return 1;
}

nomask static int cmd_work(string str) {
    string *tmp;
    object ob;
    string file;
    int flag;

    if(!str || str == "") ob = environment(owner);
    else if(str == "!") {
        flag = 1;
        ob = environment(owner);
    }
    else if(str[0] == '!') {
        flag = 1;
        str = str[1..strlen(str)];
    }
    if(!ob && !(ob = present(str, owner)))
      ob = present(str, environment(owner));
    if(!ob) return notify_fail("No target object found.\n");
    tmp = explode(file = base_name(ob), "/");
    set_cwd("/"+implode(tmp[0..sizeof(tmp)-2], "/"));
    if(flag) {
        message("system", file+".c, "+file_size(file+".c")+" bytes:\n",
          owner);
        owner->eventEdit(file+".c");
    }
    return 1;
}

nomask void write_prompt() { 
    string tmp, ret; 
    int x, y;
 
    if((ret = Prompt) == DEFAULT_PROMPT) return message("prompt", ret, owner);
    while((x = strsrch(ret, "$")) != -1) {
        if(x == strlen(ret) -1) break;
        switch(ret[x+1]) {
            case 'D': 
                if(!wizardp(owner)) break;
                if(sscanf(query_cwd(), user_path(owner->query_name())+"%s",
                  tmp)) tmp = "~"+tmp;
                else tmp = query_cwd();
                ret = replace_string(ret, "$D", tmp); 
                break;
            case 'V': case 'v':
                if(owner->query_invis()) {
                    ret = replace_string(ret, "$V", "INVIS"); 
                    ret = replace_string(ret, "$v", "invis"); 
                } 
                else if(hiddenp(owner)) { 
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
    message("prompt", ret, owner);
} 

nomask void write_ed_prompt(int y) {
  string ret;

  if( !y ) {
    if( wizardp(owner) ) ret = ":";
    else 
      ret = "\tQ)uit without saving, save and ex)it, h)elp\nCommand: ";
  }
  else if( y == -2 ) ret = "Help: ";
  else ret = "*\b";
  message("prompt", ret, owner);
}

string process_input(string str) { 
    string tmp, xtra, request; 
 
    if(!str || str == "") return ""; 
    else if(owner->query_client() &&
      member_array(owner->query_client(), SUPPORTED_CLIENTS) != -1) {
        if(sscanf(str, "<%s>%s", request, xtra)) {
            process_request(request, xtra ? xtra : "");
            return "";
        }
        else return str;
    }
    else if((tmp = do_history(str)) == "") return ""; 
    if(tmp != str) message("system", tmp+"\n", owner);
    add_history_cmd(tmp); 
    return do_alias(do_nickname(tmp));
} 
 
nomask static void process_request(string request, string xtra) {
    switch(request) {
        case "ALIAS":
            message("nofilter",
		    "<ALIAS>[n,north] [s,south] [e,east] [w,west] "
		    "[nw,northwest] [ne,northeast] [sw,southwest] "
		    "[se,southeast] [u,up] [d,down] [i,inventory] "
		    "[bio,biography] [exa,look at $*] [$',say $*] "
		    "[$:,emote $*] [l,look]\n",
		    owner);
            break;
        case "NICKNAME": message("nofilter", "<NICKNAME>\n", owner); break;
        case "USERS":
            message("nofilter",
		    "<USERS>" + implode(map_array(filter(users(), 
		    "request_vis", owner),"user_names",owner), 
		    ", ")+"\n", owner);
            break;
	case "ROOM":
            message("nofilter",
		    "<ROOM>"+ (string)environment(owner)->short()+"\n", owner);
            break;
	case "PRESENT":
            message("nofilter",
		    "<PRESENT>"+
		    implode(map_array(filter(all_inventory(environment(owner)),
		    "request_vis", owner), "user_names",owner),
                    ", ")+"\n", owner);
            break;
        default:
            message("nofilter", "<error>Request not supported.\n", owner);
            break;
	}
}

static int request_vis(object ob) {
    return (userp(ob) && !((int)ob->query_invis(owner)));
  }

static string user_names(object ob) {
    return (string)ob->query_cap_name();
  }

private static int set_cwd(string str) { 
    int x;

    if(!str || str == "") str = user_path(owner->query_name());
    if (str[<1] == '/' && str != "/") str = str[0..<2];
    replace_string(str, "//", "/"); 
    str = absolute_path(query_cwd(), str);
    if((x=file_size(str)) != -2) { 
        if(x > -1) { 
            message("system", sprintf("%s: Path is a file.\n", str), owner); 
            return 0; 
        } 
        else { 
            message("system", sprintf("%s: No such reference.\n", str), owner); 
            return 0; 
        }  
    } 
    CurrentWorkingDirectory = str;
    message("system", sprintf("%s:\n", str), owner); 
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
    return implode(map_array(explode(str, " "), "replace_nickname",
			     this_object()), " "); 
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
            message("system", "Try entering a command first.\n", owner); 
            return ""; 
        } 
        else return History[(CmdNumber-1) % HistorySize]; 
    } 
    if(sscanf(str, "%ss/%s", cmd, expr) == 2) { 
        if((tmp = do_history(cmd)) == "") return ""; 
        switch(sizeof(args = explode(expr, "/"))) { 
        case 2: 
            if((x = strsrch(tmp, args[0])) == -1) { 
                message("system", "nmsh: History expression not found.\n", 
                  owner); 
                return ""; 
            } 
            else if(!x)
              return sprintf("%s%s",args[1],tmp[strlen(args[0])..strlen(tmp)-1]);
            else return sprintf("%s%s%s", tmp[0..x-1], args[1], 
              tmp[x+strlen(args[0])..strlen(tmp)-1]); 
        case 3: 
            if(args[2] != "g") { 
                message("system", "nmsh: History syntax error.\n", 
                  owner); 
                return ""; 
            } 
            else return replace_string(tmp, args[0], args[1]); 
        default: 
            message("system", "nmsh: History syntax error.\n", owner); 
            return ""; 
        } 
    } 
    if(sscanf(str, "!%d", x) == 1) { 
        if(x < 1) x = 1+ CmdNumber + x;
        if(x < 1 || x >= 1 + CmdNumber) { 
            message("system", sprintf("Event %d not found.\n", x), 
              owner); 
            return ""; 
        } 
        if(CmdNumber - HistorySize > x) { 
            message("system", sprintf("Event %d is lost from the queue.\n",x), 
		    owner); 
            return ""; 
        } 
        else return History[(x-1) % HistorySize]; 
    } 
    else { 
        if(CmdNumber <= HistorySize)
          i=sizeof(args=map_array(History[0..CmdNumber-1],"replace_null",
            owner));
        else i=sizeof(args=map_array(History[HistoryBottom..HistorySize-1]+
          History[0..HistoryTop], "replace_null", owner));
        while(i--) if(args[i] && !strsrch(args[i], str[1..strlen(str)-1])) return args[i];           /* Tim */
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
    History = allocate(HistorySize = MIN_HISTORY_SIZE); 
} 
 
void reset_prompt() { 
    Prompt = DEFAULT_PROMPT; 
    if(!stringp(Prompt)) Prompt = "Prompt screwey> ";
    Prompt =replace_string(Prompt, "$M", mud_name()); 
    Prompt =replace_string(Prompt, "$m", lower_case(mud_name())); 
    Prompt =replace_string(Prompt, "$N", (string)owner->query_cap_name());
    Prompt =replace_string(Prompt, "$n", (string)owner->query_name());
} 

string shell_id() {
  return "Nmsh";
}

int cmd_hook(string arg) {
  switch(query_verb()) {
  case "alias":
    return cmd_alias(arg);
  case "history":
    return cmd_history(arg);
  case "nickname":
    return cmd_nickname(arg);
  case "prompt":
    return cmd_prompt(arg);
  default:
    if(wizardp(owner))
      switch(query_verb()) {
      case "cd":
	return cmd_cd(arg);
      case "nmsh":
	return cmd_nmsh(arg);
      case "pushd":
	return cmd_pushd(arg);
      case "popd":
	return cmd_popd(arg);
      case "pwd": case "cwd":
	return cmd_pwd(arg);
      case "work":
	return cmd_work(arg);
      }
    return owner->cmd_hook(arg);
  } /* 1st switch */
}

string query_cwd() { return CurrentWorkingDirectory; } 

string* query_path() {
  if(Env["PATH"])
    return explode(Env["PATH"], ":");
  else
    return DEFAULT_PATHS;
}
