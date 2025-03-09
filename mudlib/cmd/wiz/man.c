#include <mudlib.h>
#include <cmdline.h>

#undef DEBUG
#define MANPATH "/doc/man"
#define USAGE "usage:  man <page>\n"\
              "        man <section> <page>\n"\
              "        man -[kf] <keyword>\n"

int
main(string *argv, string *argv2)
{
  int section, curr_line;
  string arg, line, page;
  string* manpaths;
  string* morelines = ({});
  int i, k;
  boolean flag_k = flag("k"), flag_f = flag("f");

  if(!sizeof(argv2))
    return notify_fail(USAGE);

  arg = implode(argv2, " ");
    
  if(sscanf(arg, "%d %s", section, page) != 2)
    page = arg;

#ifdef DEBUG
  write("sect: "+section+" page: "+page+"\n");
#endif

  manpaths = explode(MANPATH, ":");

#ifdef DEBUG
  write(dump_variable(manpaths));
#endif

//  Flags k (apropos) and f (whatis)
  if(flag_k || flag_f) {
    for(k=0;k < sizeof(manpaths);k++) {
      curr_line = 1;

      if(manpaths[k][strlen(manpaths[k])-1] != '/')
	manpaths[k] += "/";

      while(line = read_file(manpaths[k] + "whatis", curr_line++, 1)) {
	if(flag_k && strsrch(line, page) != -1)
	  morelines += ({ line[0..<2] });
	if(flag_f && strsrch(line, page+" ") != -1)
	  morelines += ({ line[0..<2] });
      }
    }

    if(!sizeof(morelines))
      printf("%s: nothing appropriate.\n", page);
    else
      this_player()->more(morelines);

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
    } else
      return notify_fail("Valid sections are 1 through 10.\n");

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

string
help_desc()
{
  return "display manual pages for MudOS and mudlib defined functions";
}

string
help()
{
  return USAGE + @ENDHELP

The man command displays manual pages for the MudOS driver and mudlib
defined functions.

Options:

  -k          Search for function descriptions containing <keyword>.
  -f          Same as -k except keyword must appear by itself in the
              description of the function.
ENDHELP;
}
