/*  -*- LPC -*-  */
// groupd.c:  Tim McIntosh: 09/16/95, uses some boolean defines from config.h
//
// 
#include <mudlib.h>
#include <dirs.h>

#define DEBUG

// location of group file
#define GROUP_FILE CONFIG_DIR "/group"

inherit DAEMON;

private mapping groups;                  //  Mapping of group members

/* Prototypes */
// is 'group' really a group?
public boolean group_exists(string group);

// is 'user' member of 'group'? 
public boolean is_member(string group, string user);

// returns an array of group names that 'user' is a member of
public string * query_groups(string user);

// forces groupd to reread the groups file; returns TRUE on success
public boolean init_groups();

#ifdef DEBUG
public mapping dump_groups();
#endif

private string * expand_subgroups(string *uidlist);
private string * remove_duplicates(string *arr);

void
create()
{
  daemon::create();
  daemon::SetNoClean(1);                // don't clean up
  init_groups();
}

#ifdef DEBUG
public
mapping
dump_groups()
{
  return groups;
}
#endif

public
string *
query_groups(string user)
{
  string *ret = ({});
  string *grouplist = keys(groups);
  int i = sizeof(grouplist);

  while(i--) {
    if(is_member(grouplist[i], user))
      ret += ({ grouplist[i] });
  }

  return ret;
}
    
public
boolean
is_member(string group, string user)
{
  if(sizeof(groups[group]) == 0)                 // group includes all members
    return TRUE;

  if(member_array(user, groups[group]) == -1) return FALSE;
  return TRUE;
}

public
boolean
group_exists(string group)
{
  if(member_array(group, keys(groups)) == -1)
    return FALSE;

  return TRUE;
}

public
boolean
init_groups()
{
  int curr_line = 1;
  int i, j;
  string curr_group = 0;
  string tmp, *gplst;

  groups = ([]);

  if(file_size(GROUP_FILE) <= 0)
    log_file("groupd", "warning: group file missing\n");

  while(tmp = read_file(GROUP_FILE, curr_line++, 1)) {
    sscanf(tmp, "%s\n", tmp);                              // strip newline
    tmp = replace_string(tmp, " ", "");                    // strip spaces

    switch(tmp[0]) {
    case '#':                                              // comment
      continue;
      break;
    case '(':                                              /* ) for emacs */
      sscanf(tmp, "(%s)%s", curr_group, tmp);              // get groupname
      groups += ([ curr_group : explode(tmp, ",") ]);      // get members
      break;
    default:
      if(!curr_group) continue;                    // skip leading blank lines
      groups[curr_group] += explode(tmp, ",");             // get members
    }
  }

  /*
   * groups are basically set up, need to expand subgroups
   */
  groups = map_mapping(groups, (: expand_subgroups($2) :));

  /* 
   * now remove double elements
   */
  groups = map_mapping(groups, (: remove_duplicates($2) :));
	
  return TRUE;
}

/*
 * define a function here to use with map_mapping
 */
private
string *
expand_subgroups(string *uidlist)
{
  int i = sizeof(uidlist);
  string grp;

  while(i--) {
    if(sscanf(uidlist[i], "{%s}", grp) == 1 && group_exists(grp)) {
      uidlist -= ({ uidlist[i] });               // remove group identifier
      uidlist += groups[grp];                    // and replace with group
    }
  }

  return uidlist;
}

/*
 * define a function here to use with map_mapping
 */
private
string *
remove_duplicates(string *arr) {
  int i = sizeof(arr);
  string tmp;
  while(i--) {
    tmp = arr[i];                 // store elt
    arr -= ({ tmp });             // subtract out all copies of elt
    arr += ({ tmp });             // put 1 copy of elt back in
  }
  return arr;
}
