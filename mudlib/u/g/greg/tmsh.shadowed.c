/* -*- LPC -*- */
//  The Mud SHell or
//  Tim Mcintosh SHell or
//  This Mud Shell Heaves
//
//  tmsh.c:  Tim McIntosh (greg@viking/henry@leviathan/astasia@iastate.edu)
//
//  Hopefully a 'csh' like (eventually) MUD login shell for wizards.
//
//  One of the goals of this project is to hog as much memory as possible
//  (just kidding)...not that it doesn't :)
//  
//  FEATURES:
//	- Input/Output redirection (this is the big one) (incomplete)
//      - Shell variables (replaced on command line)
//      - Aliases
//      - Runs shell scripts (so can also have an initialization file)
//      - Multiple commands on one line separated with ';' (if defined)

//  TODO (do these if you want)
//      - make a new command line parsing function
//      - change the way commands are called and i/o is handled
//      - add looping support

#include <globals.h>

// Version number: W.X.Y.Z
//  W.X.Y is the release number
//  Z is the revision number--an 'x' after this means it is unstable
#define VERSION "0.0.4.8xS"

// define this for the ability to enter multiple commands
// on one line separated by a semicolon
// NOTE: this is causing problems with 'cat'ing a C file and piping it
//       into another command currently...
#undef CSH_CMD_LINE

// define this to be the function your commands use
#define MAIN(x) main(x)

// standard user path (paths separated by colons)
#define USER_PATH "/players/greg/open/bin"

#define  ALLOW_DEBUG			   // include debug code or not
#define ESC "@"                            // escape prefix for shell commands
#define VAR "$"                            // Denotes shell variable
#define COMMENT "#"                        // Denotes a comment statement
#define FAIL_MSG "command failed"          // What we get when command fails
#define NL "\n"                            // You tell me :)
#define WRITE(x) tell_object(owner, x)     // used to send msg to the owner

// Leave this how it is...use the above define to omit debug
#ifdef ALLOW_DEBUG
#define DEBUG(x) if(debug)\
          tell_object(owner, "%^RED%^debug tmsh:%^END%^ "+ x +"%^YELLOW%^#%^END%^\n")
#else
#define DEBUG(x)
#endif

inherit BASE;

// Global Variables
string outfile, infile, errfile, pipe;     // redirection files
string leftover;                           // used with pipes only
mapping table;                             // array of command files
mapping alias;                             // alias database
mapping vars;                              // shell variables database
mapping shellcmds;                         // built-in shell commands
int debug;                                 // debug flag
int escaped;                               // using shell cmds or not
object owner;                              // owner of this shell

// Returns the owner of this shell

object query_owner() { return owner; }

string * query_cmdpaths() {
  return (string *)owner->query_cmdpaths();
}

// Gets returns the value of a shell variable
mixed get_env(string str) {
  string *keyset;

  keyset = keys(vars);
  if(member_array(str, keyset) != -1) {
    return vars[str];
  } else {
    return owner->query_property(str);
  }
}

// Returns the current version line...

string version() {
  return "tmsh: The Mud SHell. Version "+ VERSION;
}

void create() {
  int shlvl;
  object it, prevsh;

  seteuid(getuid(this_player()));

  shlvl = 1;
  it = present("shell", this_player());
  while(it && it != prevsh) {
    int tmp;

    tmp = it->get_env("shlvl");
    if(tmp >= shlvl) shlvl = tmp + 1;

    prevsh = it;
    it = present("shell", this_player());
  }

  shellcmds = (["exit"      :  "drop",
		"version"   :  "print_version",
		"help"      :  "show_help",
		"cmds"      :  "print_table",
		"alias"     :  "define_alias",
                "debug"     :  "toggle_debug",
		"which"     :  "do_which",
		"rehash"    :  "hash_cmds",
		"set"       :  "set_env",
		"unset"     :  "set_env",
		]);

  vars = ([ "shlvl"   :  shlvl,
          ]);

  alias = ([]);
}

void init() {
  string tmp;
  object ob;

  add_action("get_cmd", "");
  owner = environment();

  ob = shadow(owner, 0);      // shadow the owner
  if(!ob)
    WRITE("WARNING: was unable to shadow owner!\n");

  tmp = file_name();
  sscanf(tmp, "%s#%*s", tmp);
  vars += (["shell"  :  tmp+".c",
            "version" : this_object()->version(),
	   "path"   :  USER_PATH,
	   "tmsh":  VERSION,
	 ]);
  this_object()->hash_cmds(0);
  WRITE(this_object()->version() +"\n");
  WRITE("Type ~help for a list of internal shell commands.\n");
  this_object()->MAIN("~/.tmshrc");
}

//  These are supposedly shadowing the player...we'll see

void
receive_message(string class, string msg)
{
  owner->receive_message("shell", class+";"+msg);
}

void
write_prompt()
{
  owner->receive_message("shell", "tmsh% ");
}

//  This is the special function called when this object is used as
//  a command to execute a script.

int MAIN(string file) {
  string text, *cmds, *lines;
  int i, j;
  object shell;

  shell = previous_object();         // Keep track of the user's shell
  if((file == "") || !file) {
    shell->stdout("Usage: tmsh <script file>\n");
    if(shell != this_object()) destruct(this_object());
    return 1;
  }
  file = "/" + shell->query_owner()->path_resolve(file);

  if(file_size(file) >= 0) {
    text = read_file(file);
    lines = explode(text, "\n");

    for(j=0; j < sizeof(lines); j++) {
      cmds = explode(lines[j], ";");

      for(i=0; i < sizeof(cmds); i++) {
        if(cmds[i])
            command(cmds[i], shell->query_owner());
      }
    }
  }

  if(shell != this_object()) destruct(this_object());
  return 1;
}

//  Some standard stuff to make this visible for now

// int id(string str) {
//  if(str == "shell") return 1;
//  return 0;
// }

//  Yes we need to carry this baby

int get() { return 1; }

//  Don't want this lying around...

int drop() {
  WRITE("tmsh: Exiting.\n");
  destruct(this_object());
}

//  toggles debug mode on and off

void toggle_debug() {
  if(debug) {
    WRITE("Debug Off.\n");
    debug = 0;
  } else {
    WRITE("Debug On.\n");
    debug = 1;
  }
}

//  Function:  do_cmd
//  Arguments: a verb 'cmd' and its arguments 'args'
//
//  This is the meat of this shell.
//  Searches the command table to find one and execute it
//  Need a shell escape in here eventually?

int do_cmd(string cmd, string args) {
  string file, unparsed, tmp, rest;
  int i, j, ret;
  int ok;

#ifdef CSH_CMD_LINE      // Multiple commands on a line separated w/a ;
  string *cmds;          // array of commands to process
  int k;
#endif

  outfile = 0; infile = 0; errfile = 0; pipe = 0; leftover = 0;
  if(args && args != "") {
    cmd += " "+ args;
    cmd = replace_string(cmd, " ; ", ";");
    sscanf(cmd, "%s %s", cmd, args);
  }

  DEBUG("=>do_cmd: "+ cmd +","+ args);
#ifdef CSH_CMD_LINE
  cmds = explode(cmd +" "+ args, ";");

  for(k = 0; k < sizeof(cmds); k++) {
    DEBUG("cmds["+ k +"]: "+cmds[k]);

    cmd = 0; args = 0; ok = 0;
    if(!cmds[k]) cmds[k] = "";
    if(!sscanf(cmds[k], " %s %s", cmd, args) &&
       !sscanf(cmds[k], "%s %s", cmd, args)) {
      cmd = cmds[k]; args = "";
    }
#endif    // CSH_CMD_LINE

  if(cmd && cmd != "") {
    cmd = replace_string(cmd, " ", "");
    cmd = cmd + ".c";
  } 

//  Check for output redirection; files are reset to 0 after command returns

  if(args) {
    unparsed = args;
    args = "";  tmp = ""; rest = "";

    if((sscanf(unparsed, "%s | %s %s", tmp, pipe, leftover) == 3) ||
       (sscanf(unparsed, "%s | %s", tmp, pipe) == 2) ||
       (sscanf(unparsed, "| %s %s", pipe, leftover) == 2) ||
       (sscanf(unparsed, "| %s", pipe) == 1)) {
      DEBUG("pipe detected");
      DEBUG("pipecmd: "+pipe);
      DEBUG("arguments: "+tmp);
      DEBUG("leftover: "+leftover);
      unparsed = tmp;
    }
    if((sscanf(unparsed, "%s > %s %s", tmp, outfile, rest) == 3) ||
       (sscanf(unparsed, "%s > %s", tmp, outfile) == 2) ||
       (sscanf(unparsed, "> %s %s", outfile, rest) == 2) ||
       (sscanf(unparsed, "> %s", outfile) == 1)) {
      DEBUG("output redirected");
      DEBUG("outfile: "+outfile);
      DEBUG("partial arguments: "+tmp);
      DEBUG("rest: "+rest);
      unparsed = tmp + rest;
    } else if((sscanf(unparsed, "%s >& %s %s", tmp, errfile, rest) == 3) ||
              (sscanf(unparsed, "%s >& %s", tmp, errfile) == 2) ||
	      (sscanf(unparsed, ">& %s %s", errfile, rest) == 2) ||
              (sscanf(unparsed, ">& %s", errfile) == 1)) {
      DEBUG("error redirected");
      DEBUG("errfile: "+errfile);
      DEBUG("partial arguments: "+tmp);
      DEBUG("rest: "+rest);
      unparsed = tmp + rest;
    } else if((sscanf(unparsed, "%s < %s %s", tmp, infile, rest) == 3) ||
              (sscanf(unparsed, "%s < %s", tmp, infile) == 2) ||
	      (sscanf(unparsed, "< %s %s", infile, rest) == 2) ||
              (sscanf(unparsed, "< %s", infile) == 1)) {
      DEBUG("input redirected");
      infile = "/"+ owner->path_resolve(infile);
      DEBUG("infile: "+infile);
      DEBUG("partial arguments: "+tmp);
      DEBUG("rest: "+rest);
      unparsed = tmp + rest;
      
      if(file_size(infile) > 0) {
        if(unparsed && unparsed != "") unparsed += " ";
        unparsed += read_file(infile);
      } else {
        WRITE("error: file "+ infile +" does not exist.\n");

#ifdef CSH_CMD_LINE
        continue;
#else
        return 1;
#endif  // CSH_CMD_LINE
      }

    }                   // if((sscanf(...pipe...

    args = unparsed;    // we've hacked everything out; this is the real args
    DEBUG("hacked_args: "+args);
  }                     // if(args) {

  DEBUG("command was: "+ cmd);
  DEBUG("args were: "+ args);

//  First check command table for the command

  file = this_object()->search_table(cmd);
  
  if(file) {
    ret = file->MAIN(args);
    DEBUG("ret ="+ret);
    if(!ret) {
      sscanf(cmd, "%s.c", cmd);
      WRITE(cmd +": "+ FAIL_MSG + NL);
    }
#ifdef CSH_CMD_LINE
    continue;
#else                // If the command was found, we're done
    return 1;
#endif
  }

//  Otherwise check the full file path

  DEBUG("Searching for command...");
  file = "/"+owner->path_resolve(cmd);
  DEBUG("File was: "+file);

  if(file_size(file) > 0) {
    ret = file->MAIN(args);
    if(!ret) {
      sscanf(cmd, "%s.c", cmd);
      WRITE(cmd +": "+ FAIL_MSG + NL);
    }
#ifdef CSH_CMD_LINE
      continue;
#else                        // If the file was found we're done
      return 1;
#endif
  }
  sscanf(cmd, "%s.c", cmd);
  WRITE(cmd + ": command not found.\n");

#ifdef CSH_CMD_LINE   // Matches the 'for' statement at the top
  }
#endif

  return 1;
}            // end of do_cmd

//  Function: get_cmd
//  Argument: args
//
//  Here's where we start after the command is entered from the
//  command line.  This returns the value of do_cmd

int get_cmd(string args) {
  string verb, tmp, *names;
  int ret, i;

  verb = query_verb();

//  For getting at previous shells

  if(verb == ESC + "return") {
    escaped = 0;
    WRITE("Returning to top shell."+NL);
    return 1;
  } else if(verb == ESC + "escape") {
    escaped = 1;
    WRITE("Escaped to previous shell. Type "+ESC+"return to return."+NL);
    return 1;
  }

  if(escaped) {
    WRITE("[in subshell]"+NL);
    return 0;
  }

  DEBUG("verb was: " + verb);

//  If verb is a comment, just return now
  if(verb == COMMENT) return 1;

//  Expand shell variables where appropriate
//  This will be a problem when something does query_verb()
  if((sscanf(verb, VAR + "%s", tmp) == 1) && vars[tmp]) {
    verb = vars[tmp];
  }

//  Expand variables in the arguments recursively; if the variable is 
//  not found, just strip off the variable indicator
  if(!args) args = "";
  if(strsrch(args, VAR) >= 0) {
    names = keys(vars);
    for(i = 0; i < sizeof(names); i++) {
        if(stringp(vars[names[i]]))
	  args = replace_string(args, VAR + names[i], vars[names[i]]);
    }
  }

//  Check for built-in commands
  names = keys(shellcmds);
  i = member_array(verb, names);
  if(i != -1) {
    call_other(this_object(), shellcmds[verb], args);
    return 1;
  }

//  Check for aliases
  names = keys(alias);
  i = member_array(verb, names);
  if(i != -1)
    verb = alias[verb];

  return do_cmd(verb, args);
}

//  Function: hash_cmds
//
//  Sets up the table of commands, as given by path.
//  The table is a mapping with each path as a key, and each element of
//  the mapping is an array of command files in that directory.

void hash_cmds(string one_path) {
  string *dir, *paths;
  int i,j,k;

  paths = explode(vars["path"], ":");

  if(!one_path || one_path == "") {
    table = ([]);
  } else {
    if(member_array(one_path, paths) != -1) {
      paths = ({ one_path });
    } else {
      WRITE("rehash: directory not in path\n");
      return;
    }
  }

  for(j=0; j < sizeof(paths); j++) {
    if(paths[j][strlen(paths[j]) - 1] != '/') paths[j] += "/";

    if(member_array(paths[j], keys(table)) == -1) {
      table += ([ paths[j] : ({ }) ]);     // add this dir if its not there
    }
    table[paths[j]] = ({ });    // clear this directory

    dir = get_dir(paths[j]);    // add the file entries in the dir
    for(k=0; k < sizeof(dir); k++) {
      table[paths[j]] += ({ dir[k] });
      i++;
    }
  }
  DEBUG("Command table hashed.");
}

// Function: search_table
// Argument: command
//
// Finds a command file in the table and returns it

string search_table(string command) {
  string *key;
  int i,j;

  key = keys(table);
  for(i=0; i < sizeof(key); i++) {
    for(j=0; j < sizeof(table[key[i]]); j++) {
      if(table[key[i]][j] == command) {
	DEBUG("=>search_stk: "+ table[key[i]][j]);
	return key[i] + table[key[i]][j];
      }
    }
  }
  return 0;
}

//  A debugging aid for now...list the commands in the table in sloppy form

int print_table() {
  string cmd, line;
  string *key;
  int i,j;

  line = "";
  key = keys(table);
  for(i=0; i < sizeof(key); i++) {
    for(j=0; j < sizeof(table[key[i]]); j++) {
      line += table[key[i]][j] + "\t";
      if(strlen(line) > 70) {
	WRITE(line + "\n");
	line = "";
      }
    }
  }
  return 1;
}

//  Function: stdout
//  Argument: a string to be outputted
//
//  This gets called by commands, etc. when standard output is desired
//  This figures out what to do with standard output

void stdout(string str) {
  string file;

  DEBUG("=>stdout: begin function.");
  if(!pipe && !outfile)
    WRITE(str);
  if(outfile) {
    file = owner->path_resolve(outfile);
//  Put a flag in here later to prevent/allow overwriting files on accident
    write_file(file, str);
  }
  if(pipe) {
    if(!leftover) leftover = "";
//  Need to put any pipes after the new args, so they go w/the right cmd
    if(strsrch(leftover, "|") >= 0) {
      str = str +" "+ leftover;
    } else {
      str = leftover +" "+ str;
    }
    do_cmd(pipe, str);
  }
  return;
}

//  Function: stderr
//  Arguements: a string
//
//  Same idea as stdout.
//  What to do with standard error

void stderr(string str) {
  string file;

  if(errfile) {
    file = owner->path_resolve(errfile);
    write_file(file, str);
  } else {
    WRITE("error: "+ str);
  }
}

//  Function: help
//  Argument: a string (optional)

//  Without an argument, show the help.
//  If passed a string, try to show the help for that command.

void show_help(string str) {
  string cmd;

  if(str && str != "") {
    cmd = this_object()->search_table(str + ".c");
    DEBUG("=>show_help: str="+str+" cmd="+cmd+"\n");
    if(cmd) {
      cmd = cmd->help();
      if(cmd[0] == '/') cmd = read_file(cmd);
      WRITE(cmd);
      return;
    } else {
      WRITE("No help found for "+ str +"\n");
      return;
    }
  }

  WRITE("SHELL COMMANDS:\n"+
    "alias      set/unset/list shell aliases.\n"+
    "help       with an argument, help on that topic. Otherwise this screen.\n"+
    "rehash     rehash the command table, or specific path."+NL+
    "which      show the location of a command file."+NL+
    "version    print the version number."+NL+
    ESC+"escape     escape to previous shell level."+NL+
    ESC+"return     return from a subshell."+NL+
    "debug      turn on/off debugging mode."+NL+
    "cmds       list the commands in the command table."+NL+
    "set/unset  set or remove shell variables."+NL+
    "exit       exit from tmsh."+NL
	);
}

// Function: path
// Argument: a path (optional)

// As it is now, this isn't a shell command, it is called externally.
// I can't remember why I did this, but as long as there are no problems
// I'm going to leave it

void path(string str) {
  string flag, *paths;

  if(!str) {
    WRITE("Current path is:\n"+ vars["path"] +"\n");
    return;
  }
  if(sscanf(str, "-%s %s", flag, str) != 2) {
    WRITE("Usage: path [-a] [-r] <command path>\n");
    return;
  }
  paths = explode(vars["path"], ":");
  if(flag == "a") {
    paths += ({ str });
  } else if(flag == "r") {
    paths -= ({ str });
  }
  vars["path"] = implode(paths, ":");
  this_object()->hash_cmds(0);
}

//  Function: set_env
//  Argument: variable and value

//  A shell command that sets an environment variable to a value

int set_env(string args) {
  string *names, verb, str, val;
  int i;

  if(!args) args = "";
  if((sscanf(args, "%s %s", str, val) == 0) && (args == "")) {
    DEBUG("str: "+ str +" val: "+ val);
    WRITE("Shell Variables:\n");
    WRITE("----- ----------\n");
    names = keys(vars);
    for(i=0; i < sizeof(vars); i++) {
      WRITE(names[i] +"\t"+ vars[names[i]] +"\n");
    }
    WRITE("\nCompiled Options:\n");
#ifdef CSH_CMD_LINE
    WRITE("CSH_CMD_LINE--allows multiple commands on a line with a ';'\n");
#endif
#ifdef ALLOW_DEBUG
    WRITE("ALLOW_DEBUG--Debugging code compiled in.\n");
#else
    WRITE("Debugging code omitted.\n");
#endif
    WRITE("Variable indicator = "+ VAR +"\n");
    WRITE("Comment indicator = "+ COMMENT +"\n");
    WRITE("Shell command prefix = "+ ESC +"\n");
    return 1;
  }
  if(!str) str = args;
  verb = query_verb();

  if(verb == "set") {
    vars += ([ str : val ]);
  } else if(verb == "unset") {
    map_delete(vars, str);
  }
  if(str == "path") this_object()->hash_cmds(0);
  DEBUG("set_env success.");
  return 1;
}

//  Defines an alias

int define_alias(string args) {
  string *names, str, val;
  int i;

  DEBUG("alias: args: "+ args);

  if(!args || args=="") {
    names = keys(alias);
    write("Alias\t\tDefinition\n");
    write("-----\t\t----------\n");
    for(i=0; i < sizeof(names); i++) {
      write(names[i]+"\t\t"+alias[names[i]]+"\n");
    }
    return 1;
  }

  if(sscanf(args, "%s %s", str, val) != 2) {
    str = args;
    map_delete(alias, str);
    DEBUG("alias: alias removed");
    return 1;
  } else {
    alias += ([ str : val ]);
    DEBUG("alias: alias added");
    return 1;
  }
}

void print_version() {
  WRITE(this_object()->version() +"\n");
  return;
}

// do_which:  shell command.
//
//            Prints the command
void do_which(string cmd) {
  string *names, file;

  names = keys(shellcmds);

  if(member_array(cmd, names) != -1) {
    stdout("Built-in shell command."+NL);
  } else {
    if(file = this_object()->search_table(cmd+".c"))
      stdout(file+NL);
    else
      stdout(cmd+": not found."+NL);
  }
  return;
}
