/*  -*- LPC -*-  */
/*
 * Shell Inheritable
 * (C)1996 Cameron Kaiser "Evergreen"
 * 
 * This code is governed by the UltraLib usage agreement.
 * Removal of this header is illegal. All rights reserved.
 *
 * 02.26.96  Tim     :Modified to produce shell compatible with new user.c
 */

#include <mudlib.h>
#include <daemons.h>
#define ESC ""
#define MSG_SHELL "system"

private mapping aliases;	/* Aliases */
private mapping env;            /* Environment variables */
private object owner;           /* Shell Owner */

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

string shell_prompt();                            // return normal prompt
string shell_ed_prompt();                         // return prompt for ed
string shell_process_input(string input);         // process player input
string shell_filter_message(string msgclass, string msg); // filter msgs
int    shell_cmd_hook(string verb, string arg);   // shell can catch cmds
// called from init_player() to give the shell a chance to perform any
// setup operations
void   shell_init();
int cmd_set(string args);
int cmd_alias(string args);
int cmd_cd(string args);

int clean_up() { return 1; } // Ha! Wrong!

string shell_id() { return "Prototype Shell"; }
// Put the name of the shell here

// called from init_player() in the user object
void
shell_init()
{
  if(owner) return;

  owner = previous_object();
  seteuid( geteuid(owner) );

  aliases = (mapping)owner->get_aliases();
  // Get aliases from user

  env = (mapping)owner->get_environment();
  // Get environment variables from user
}

string
shell_filter_message(string msgclass, string message)
{
  string ttype = env["TERM"];

  if( !ttype ) ttype = "dumb";

  return TERMCAP_D->termcap_format_line(message, ttype);
  // this shell has no class interest
}

private
string
lglob(string input)
{ 
  int idx;
  
  if (env["noglob"]) return input;
  // don't if there's noglob
  
  input = absolute_path(owner->query_cwd(), input);

  if( idx = strsrch(input, '*') < 0 )
    idx = strsrch(input, '?');

  if( input[idx - 1] == '/' )
    input = implode(glob(input[0..idx - 2], explode(input[idx..], "/")), "/");
  else
    input = implode(glob(input[0..idx - 1], explode(input[idx..], "/")), "/");

  return "/" + input;
}

string
shell_process_input(string input)
{
  string* words = explode(input, " ");

  if( !sizeof(words) ) return "";

  // Alias expansion
  if( aliases[words[0]] )
    words[0] = aliases[words[0]];

  for(int i = 0; i < sizeof(words); i++) {
    string replace;
    int idx;
    
    // Shell variable expansion
    if( (idx = strsrch(words[i], '$')) >= 0 &&
        (replace = env[words[i][idx + 1..]]) )
    {
      if(!idx)
	words[i] = replace;
      else if(words[i][idx - 1] != '\\')
	words[i] = words[i][0..idx - 1] + replace;
    }

    if( strsrch(words[i], '*') >= 0 || strsrch(words[i], '?') >= 0 )
      words[i] = lglob(words[i]);
  }

  return implode(words, " ");
}

int
shell_cmd_hook(string verb, string args)
{
  switch(verb) {
  case "set": case "unset":
    return cmd_set(args);
  case "alias": case "unalias":
    return cmd_alias(args);
  case "cd":
    return cmd_cd(args);
  default:
    return 0;
  }
}

int
cmd_set(string arg)
{
  if( !arg || arg == "" ) {
    message(MSG_SHELL,
	    "  variable                    value\n"
	    "  --------                    -----\n", owner);

    foreach(string key, mixed val in env)
      message(MSG_SHELL, sprintf("  %:-25s   %:-50O\n", key, val), owner);
  } else if( query_verb() == "set" ) {
    string key;
    mixed val;

    if( sscanf(arg, "%s %d", key, val) != 2 &&
        sscanf(arg, "%s %s", key, val) != 2 )
      return notify_fail("usage: set | set <variable name> <value>\n");

    env[key] = val;
  } else if( query_verb() == "unset" ) {
    if( env[arg] )
      map_delete(env, arg);
    else
      return notify_fail("unset: no such environment variable.\n");
  }

  return 1;
}

int
cmd_alias(string arg)
{
  if( !arg || arg == "" ) {
    message(MSG_SHELL,
	    "  alias                       long form\n"
	    "  -----                       ---------\n", owner);

    foreach(string key, string val in aliases)
      message(MSG_SHELL, sprintf("  %:-25s   %:-50O\n", key, val), owner);
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

int
cmd_cd(string arg)
{
  if( !arg || arg == "" ) arg = "~";

  arg = absolute_path(owner->query_cwd(), arg);

  if( directory_exists(arg) ) {
    if( !owner->set_cwd(arg) )
      return notify_fail("cd: unable to change cwd\n");
  } else {
    return notify_fail("cd: no such directory\n");
  }

  return 1;
}
