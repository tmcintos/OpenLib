#include <mudlib.h>
#include <cmdline.h>

#undef DEBUG
#define MANPATH "/doc/man"
#define USAGE "usage:  man [-k] <page>\n        man [-kf] <section> <page>"

int
_main(string arg, int argc, string *argv, string *flags)
{
  int section, i, k;
  int curr_line;
  string page;
  string tmpfile;
  string line;
  string *manpaths;
  int flag_k, flag_f;

  if(!arg)
    return notify_fail(USAGE +"\n");

  if(sscanf(arg, "%d %s", section, page) != 2)
    page = arg;

#ifdef DEBUG
  write("sect: "+section+" page: "+page+"\n");
#endif

  manpaths = explode(MANPATH, ":");
  flag_k = flag(flags, "k");
  flag_f = flag(flags, "f");

#ifdef DEBUG
  write(dump_variable(manpaths));
#endif

//  Flags k (apropos) and f (whatis)
  if(flag_k || flag_f) {
    tmpfile = TMP_DIR +"/mantmp"+random(9999)+".tmp";     // temporary file
    rm(tmpfile);
    for(k=0;k < sizeof(manpaths);k++) {
      curr_line = 1;
      if(manpaths[k][strlen(manpaths[k])-1] != '/')
	manpaths[k] += "/";
      while(line = read_file(manpaths[k]+"whatis", curr_line++, 1)) {
	if(flag_k && strsrch(line, page) != -1)
	  write_file(tmpfile, line);
	if(flag_f && strsrch(line, page+" ") != -1)
	  write_file(tmpfile, line);
      }
    }
    this_player()->more(tmpfile);
    return 1;
  }

  if(section) {
    if(section < 10) {
      for(k=0;k < sizeof(manpaths);k++) {
	if(manpaths[k][strlen(manpaths[k])-1] != '/')
	  manpaths[k] += "/";
	if(file_size(manpaths[k]+"man"+section+"/"+page+"."+section) >= 0) {
	  this_player()->more(manpaths[k] +"man"+ section
			      +"/"+ page +"."+ section);
	  return 1;
	}
      }
      return notify_fail("No entry for " + page
			 + " in section " + section + " of the manual\n");
    } else {
      return notify_fail("Valid sections are 1 through 10.\n");
    }
  } else {
    for(k=0;k < sizeof(manpaths);k++) {
      if(manpaths[k][strlen(manpaths[k])-1] != '/')
	manpaths[k] += "/";
      for(i=0;i < 10;i++) {
	if(file_size(manpaths[k] + "man" + i +"/"+ page +"."+ i) >= 0) {
	  this_player()->more(manpaths[k] + "man" + i + "/" + page + "." + i);
	  return 1;
	}
      }
    }
    return notify_fail("No manual entry for " + page +"\n");
  }
}
