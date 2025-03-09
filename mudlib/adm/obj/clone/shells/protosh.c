/*  -*- LPC -*-  */
/*
 * Shell Inheritable
 * (C)1996 Cameron Kaiser "Evergreen"
 * 
 * This code is governed by the UltraLib usage agreement.
 * Removal of this header is illegal. All rights reserved.
 *
 * 02.26.96  Tim     Modified to produce shell compatible with new user.c
 * 02.27.96  Tim     Extensive modifications to shape into present form
 * 02.28.96  Tim     
 * 09.22.96  Tim     Made LPC eval work..2 liner.  Changed to use to_string()
 *                   Man this code is ugly!
 */

#include <mudlib.h>
#include <dirs.h>
#include <daemons.h>
#include <shells.h>
#include <time.h>

#define ESC ""
#define MSG_SHELL "system"

inherit INHERIT_DIR "/time";

private static mapping aliases;        /* Aliases */
private static mapping env;            /* Environment variables */
private static mapping history;        /* History list */
private static string current_dir;
private static int hist_start;         /* History start */
private static int hist_end;           /* History end */
private static object owner;           /* Shell Owner */

/*
 * You must clone this object first before you start referencing it.
 *  This is because it uses global variables. For example:
 *
 *  object shell;
 *
 *  shell = clone_object(SHELL);
 * 
 *  This insures no conflict between concurrent sessions.
 */

/*
 * Applies
 */
string shell_id();                                // returns shell id string
object shell_init(object ob);      // called by logind to setup shell
/*
 * Masked Functions
 */
void write_prompt();                            // return normal prompt
void write_ed_prompt(int mode);                 // return prompt for ed
string process_input(string input);             // process player input
int cmd_hook(string arg);                       // shell can catch cmds

string query_cwd();                             // required for wizards
string* query_path();                           // ditto
/*
 * Extra Functions
 */
private string process_arguments(string args);    // process args for cmd_hook
private int check_syntax(string line);            // check syntax on string
private string* get_words(string line);           // break line into words
private int escaped(string str, char ch);         // is 'ch' escaped in 'str'?
private string substitute_vars(string word);      // subst env vars into word
private string substitute_history(string word);   // subst hist strs into word
private string lglob(string word);                // local glob function
private string remove_slashes(string str);        // remove escape chars
private int cmd_set(string args);                 // env var set user command
private int cmd_alias(string args);               // alias set user command
private int cmd_cd(string args);                  // change dir user command
private int cmd_history();                        // display history
private void add_history(string str);             // add to history list

string
shell_id()
{
  return "Prototype Shell";  // Put the name of the shell here
}

// called from init_player() in the user
// should set up the shell
// previous_object() is the user object
object
shell_init(object ob)
{
  string tmp;

  if(owner) return;
  if(!(owner = shadow(ob))) return 0;

  current_dir = user_cwd(owner->query_name());
  history = ([]);
  hist_start = hist_end = 1;

  aliases = (mapping)owner->get_aliases();
  // Get aliases from user

  env = (mapping)owner->get_environment();
  // Get environment variables from user

  tmp = file_name(this_object());
  env["SHELL"] = tmp[0..strsrch(tmp, '#') - 1];
  env["version"] = shell_id();
  // File name of the shell is handy to have

  if( !env["PATH"] )
    env["PATH"] = "/cmd/player:/adm/cmd/player:/cmd/wiz:/adm/cmd/wiz:"
                  "/cmd/adm:/adm/cmd/adm";
  return owner;
}

// basically a watered down version of get_words() that makes sure
// everything will be ok
//
private
int
check_syntax(string line)
{
  int double_quotes, left_quote, right_quote;
  int escaped;

  for(int i = 0; i < sizeof(line); i++) {
    if(line[i] == '\\' && !escaped) {
      escaped = 1;
      continue;
    } else if(escaped) {
      escaped = 0;
      continue;
    }

    if( (double_quotes && line[i] != '"') || (left_quote && line[i] != '`') ||
	(right_quote && line[i] != '\'') )
      continue;
    // if we're in the middle of a quoted string, ignore anything except a
    // closing quote

    switch(line[i]) {
    case '"':
      double_quotes = !double_quotes;
      break;
    case '`':
      left_quote = !left_quote;
      break;
    case '\'':
      right_quote = !right_quote;
      break;
    }
  }
  // if there are unmatched quotes, fail
  if(double_quotes || left_quote || right_quote) return 0;

  return 1;
}

// Breaks a string into an array of words.
// Quotes force the enclosed string to become one word--the quotes are
// left on either end so they may be interpreted later if wanted.
// Any character including a quote may be escaped with the backslash (\)
// which causes the next character to be ignored. In all other cases
// the input is split into words at spaces (multiple spaces are ignored).
private
string*
get_words(string line)
{
  int double_quotes, left_quote, right_quote, escaped;
  string* words = ({});
  int idx;

  for(int i = 0; i < sizeof(line); i++) {
    if(line[i] == '\\' && !escaped) {
      escaped = 1;
      continue;
    } else if(escaped) {
      escaped = 0;
      continue;
    }

    if( (double_quotes && line[i] != '"') || (left_quote && line[i] != '`') ||
	(right_quote && line[i] != '\'') )
      continue;
    // if we're in the middle of a quoted string, ignore anything except a
    // closing quote

    switch(line[i]) {
    case '"': /* Double Quote */
      if(!double_quotes) {
	if(idx != i) {
	  words += ({ replace_string(line[idx..i-1], " ", "") });
	  idx = i+1;
	}
	idx = i;
      } else {
	words += ({ line[idx..i] });
        idx = i+1;
      }
      double_quotes = !double_quotes;
      break;
    case '`': /* Left Quote */
      if(!left_quote) {
	if(idx != i) {
	  words += ({ replace_string(line[idx..i-1], " ", "") });
	  idx = i+1;
	}
	idx = i;
      } else {
	words += ({ line[idx..i] });
        idx = i+1;
      }
      left_quote = !left_quote;
      break;
    case '\'': /* Right Quote */
      if(!right_quote) {
	if(idx != i) {
	  words += ({ replace_string(line[idx..i-1], " ", "") });
	  idx = i+1;
	}
	idx = i;
      } else {
	words += ({ line[idx..i] });
        idx = i+1;
      }
      right_quote = !right_quote;
      break;
    case ' ': /* SPACE */
      if(idx != i) {
	words += ({ replace_string(line[idx..i-1], " ", "") });
	idx = i+1;
      }
    }
  }
  if(idx < sizeof(line))
    words += ({ replace_string(line[idx..], " ", "") });

  return words - ({ "" });
}

// returns index of 'ch' if 'ch' is escaped with a backslash in 'str',
// returns -1 if 'ch' is not in 'str'; returns 0 otherwise
private
int
escaped(string str, char ch)
{
  int idx = strsrch(str, ch);

  if( idx < 0 ) return -1;

  // found the character escaped
  if( idx && str[idx - 1] == '\\' )
    return idx;

  // found, but not escaped
  return 0;
}

//
// recursively substitutes environment variables into 'word'
// returns 0 on fail
private
string
substitute_vars(string word)
{
  string var, rest, replace;
  int idx = escaped(word, '$');

  if( idx == -1 ) return word;

  if( idx ) {  /* if it was escaped */
    if( !strlen(word[idx + 1..]) )
      return word[0..idx];
    else {
      rest = substitute_vars(word[idx + 1..]);
      if( !rest ) return 0;
      return word[0..idx] + rest;
    }
  }

  idx = strsrch(word, '$');

  if( sscanf(word[idx..], "${%s}%s", var, rest) == 2 ) {
    replace = env[var];
    if( !replace ) return 0;

    if( escaped(rest, '$') < 1) {
      rest = substitute_vars(rest);
      if( !rest ) return 0;
      return sprintf("%s%s%s", (idx ? word[0..idx - 1] : ""), replace, rest);
    } else
      return sprintf("%s%s%s", (idx ? word[0..idx - 1] : ""), replace, rest);
  } else {
    replace = env[word[idx + 1..]];
    if( !replace ) return 0;
    return sprintf("%s%s", (idx ? word[0..idx - 1] : ""), replace);
  }
}

//
// recursively substitutes history strings into 'word'
// returns 0 on fail
private
string
substitute_history(string word)
{
  string var, rest, replace;
  int idx = escaped(word, '!');
  string* vals;
  int i;

  if( idx == -1 ) return word;

  if( idx ) {  /* if it was escaped */
    if( !strlen(word[idx + 1..]) )
      return word[0..idx];
    else {
      rest = substitute_history(word[idx + 1..]);
      if( !rest ) return 0;

      return word[0..idx] + rest;
    }
  }

  idx = strsrch(word, '!');
  vals = values(history);

  if( sscanf(word[idx..], "!{%s}%s", var, rest) == 2 ) {
    if( to_int(var) )
      replace = history[to_int(var)];
    else {
      vals = filter(vals, (: $1[0..$(sizeof(var)-1)] == $(var) :));
      if( !sizeof(vals) ) return 0;
      replace = vals[0];
    }
    if( !replace ) return 0;

    if( escaped(rest, '!') < 1) {
      rest = substitute_history(rest);
      if( !rest ) return 0;

      return sprintf("%s%s%s", (idx ? word[0..idx - 1] : ""), replace, rest);
    } else
      return sprintf("%s%s%s", (idx ? word[0..idx - 1] : ""), replace, rest);
  } else {
    var = word[idx + 1..];
    if( to_int(var) )
      replace = history[to_int(var)];
    else {
      vals = filter(vals, (: $1[0..$(sizeof(var)-1)] == $(var) :));
      if( !sizeof(vals) ) return 0;
      replace = vals[0];
    }
    if( !replace ) return 0;

    return sprintf("%s%s", (idx ? word[0..idx - 1] : ""), replace);
  }
}

// Do globbing on a string.
// This assumes that either a * or ? is in the string
private
string
lglob(string input)
{ 
  int idx;

  if (env["noglob"]) return input;     // don't if there's noglob

  input = absolute_path(query_cwd(), input);

  if( (idx = strsrch(input, '*')) < 0 )
    idx = strsrch(input, '?');

  idx = strsrch(input[0..idx], '/', -1);

  if( idx )
    input = implode(glob(input[0..idx - 1],
			 explode(input[idx+1..], "/")), " ");
  else
    input = implode(glob("", explode(input[1..], "/")), " ");

  return input;
}

//
// Removes non-escaped backslashes from 'str'
//
private
string
remove_slashes(string str)
{
  for(int i = 0; i < sizeof(str); i++)
    if( str[i] == '\\' )
      str[i..i] = "";

  return str;
}

// called from process_input() in user
// argument is input as recieved by the driver
// should return a string for the driver to use
string
process_input(string input)
{
  string* words = explode(input, " ");

  if( !sizeof(words) ) return "";

  // Say command substitution
  if( words[0][0] == '\'' )
    words[0][0..0] = "say ";

  // Alias expansion
  if( aliases[words[0]] )
    words[0..0] = get_words(aliases[words[0]]);
  
  // History expansion on verb
  input = substitute_history(words[0]);

  if( input )
    words[0] = input;

  return implode(words, " ");
}

// Called from cmd_hook() in the user
// argument is the argument list passed to cmd_hook
// should process and return a string to be used as the argument list
// returning 0 indicates bad syntax
private string
process_arguments(string args)
{
  string* words;

  if( !check_syntax(args) )
    return notify_fail("syntax error in expression.\n"), 0;

  words = get_words(args);

  if( !sizeof(words) ) return "";

  /*
   * Process each word
   */
  for(int i = 0; i < sizeof(words); i++) {
    int do_history = 1,
        do_vars = 1,
        do_glob = 1,
        do_lpc_eval = 0;
    string tmp;
    mixed ret;

    /*
     * Check for quotes and see what to do
     */
    switch( words[i][0] ) {
    case '\'':                   /* do no expansions */
      do_history = do_vars = do_glob = 0;
      tmp = words[i][1..<2];
      break;
    case '`':                    /* evaluate lpc code; don't glob */
      do_lpc_eval = !(do_glob = 0);
      tmp = words[i][1..<2];
      break;
    case '"':                    /* only do history & variable substitution */
      do_glob = 0;
      tmp = words[i][1..<2];
      break;
    default:
      tmp = words[i];
    }

    // History Expansion
    if(do_history) {
      tmp = substitute_history(tmp);
      if( !tmp ) return notify_fail("no such history event\n"), 0;
    }

    // Shell variable expansion
    if(do_vars) {
      tmp = substitute_vars(tmp);
      if( undefinedp(tmp) )
        return notify_fail("no such environment variable\n"), 0;
    }

    // LPC code evaluation
    if(do_lpc_eval)
     tmp = to_string(eval_string("return "+tmp));

    // Filename Globbing
    if( do_glob && (!escaped(tmp,'?') || !escaped(tmp,'*')) )
      tmp = lglob(tmp);

    words[i] = remove_slashes(tmp);
  }
  return implode(words, " ");
}

// should return 1 if a successful command was executed
int
cmd_hook(string args)
{
  string verb = query_verb();

  if(previous_object() != owner) return 0;

  if(!(args = process_arguments(args))) return 0;

  // not much use putting history command into history buffer
  if(verb != "history")
    add_history( verb + (args ? " " + args : "") );
  else
    return cmd_history();

  switch(verb) {
  case "set": case "unset":
    return cmd_set(args);
  case "alias": case "unalias":
    return cmd_alias(args);
  case "cd":
    return cmd_cd(args);
  default:
    return owner->cmd_hook(args);
  }
}

// should return a prompt to be written to the user
void
write_prompt()
{
  string prompt = env["PROMPT"];
  string cwd;

  if(!prompt) return message("prompt", "Dys% ", owner);

  if( strsrch(prompt, "%~") >= 0 ) {
    cwd = replace_string(query_cwd(),
			 user_cwd(owner->query_name()), "~");
    prompt = replace_string(prompt, "%~", cwd);
  }
  if( strsrch(prompt, "%/") >= 0 )
    prompt = replace_string(prompt, "%/", query_cwd());
  if( strsrch(prompt, "%m") >= 0 )
    prompt = replace_string(prompt, "%m", mud_name());
  if( strsrch(prompt, "%d") >= 0 )
    prompt = replace_string(prompt, "%d", get_date(time(), DATESTR_DAY));
  if( strsrch(prompt, "%w") >= 0 )
    prompt = replace_string(prompt, "%w", get_date(time(), DATESTR_MONTH));
  if( strsrch(prompt, "%Y") >= 0 )
    prompt = replace_string(prompt, "%Y", get_date(time(), DATESTR_YEAR));
  if( strsrch(prompt, "%n") >= 0 )
    prompt = replace_string(prompt, "%n", owner->query_cap_name());
  if( strsrch(prompt, "%t") >= 0 )
    prompt = replace_string(prompt, "%t", get_date(time(), DATESTR_TIME));
  if( strsrch(prompt, "%D") >= 0 )
    prompt = replace_string(prompt, "%D", get_date(time(), DATESTR_SHORT));
  message("prompt",  prompt, owner);
}

// Called from user when in edit
// should return a prompt to display
void
write_ed_prompt(int mode)
{
  string p;

  switch(mode) {
  case 0:
    p = ": ";
    break;
  case -2:
    p = "";
    break;
  default:
    p = "*\b";
  }
  message("prompt", p, owner);
}

//
// user command for setting environment variables
//
private
int
cmd_set(string arg)
{
  if( !arg || arg == "" ) {
    string *lines = ({ "  variable     value",
		       "  --------     -----" });

    foreach(string key, mixed val in env)
      lines += ({ sprintf("  %-10s   %-s", key, to_string(val)) });
    owner->more(lines);
  } else if( query_verb() == "set" ) {
    string key;
    mixed val;

    if( sscanf(arg, "%s %d", key, val) != 2 &&
        sscanf(arg, "%s %s", key, val) != 2 )
      return notify_fail("usage: set | set <variable name> <value>\n");

    key = replace_string(key, " ", "");  // strip spaces from key names

    if( key == "PATH" ) {
      foreach(string path in explode(val, ":"))
	if( !CMD_D->hashed_path(path) ) CMD_D->hash_path(path);
    }

    env[key] = val;
    printf("%s=%s", key, to_string(val));
  } else if( query_verb() == "unset" ) {
    sscanf(arg, "%s", arg);
    if( !undefinedp(env[arg]) )
      map_delete(env, arg);
    else
      return notify_fail("unset: no such environment variable.\n");
    printf("Removed variable %s.\n", arg);
  }

  return 1;
}

//
// user command for setting aliases
//
private
int
cmd_alias(string arg)
{
  if( !arg || arg == "" ) {
    string *lines = ({ "  alias        long form",
		       "  -----        ---------" });

    foreach(string key, string val in aliases)
      lines += ({ sprintf("  %-10s   %-s", key, val) });
    owner->more(lines);
  } else if( query_verb() == "alias" ) {
    string key, val;

    if( sscanf(arg, "%s %s", key, val) != 2 )
      return notify_fail("usage: alias | alias <short> <long>\n");

    aliases[key] = val;
  } else if( query_verb() == "unalias" ) {
    if( aliases[arg] )
      map_delete(aliases, arg);
    else
      return notify_fail("unalias: no such alias.\n");
  }

  return 1;
}

//
// user command for changing the current working directory
//
private
int
cmd_cd(string arg)
{
  if( !arg || arg == "" ) arg = "~";

  arg = absolute_path(query_cwd(), arg);

  if( directory_exists(arg) ) {
    current_dir = arg;
  } else {
    return notify_fail("cd: no such directory\n");
  }

  return 1;
}

private
int
cmd_history()
{
  string* lines = ({});
  foreach(int key in sort_array(keys(history), 1))
    lines += ({ sprintf("%:4d  %s", key, history[key]) });
  owner->more(lines);
  return 1;
}

private
void
add_history(string str)
{
  int maxhist = (env["history"] ? env["history"] : 50);

  for(; hist_start <= hist_end - maxhist; hist_start++)
    map_delete(history, hist_start);

  history += ([ hist_end++ : str ]);
}

string query_cwd() { return current_dir; }

string*
query_path()
{
  if(env["PATH"])
    return explode(env["PATH"], ":");
  else
    return DEFAULT_PATHS + WIZARD_PATHS;
}
