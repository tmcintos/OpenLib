/*  -*- LPC -*-  */
#include <command.h>
#include <cmdline.h>

#define USAGE "usage: clone [-n] <filename>\n"

int
_main(string *argv, string *argv2)
{
  object ob;
  string file, tmp;
  int err;
  boolean optn = flag("n");

  if (!sizeof(argv2)) {
    return notify_fail(USAGE);
  } else {
    file = RESOLVE_PATH(argv2[0]);
  }
  
  if((file_size(file) < 0) && (file_size(file+".c") < 0))
    return notify_fail("clone: file does not exist.\n");
  
  ob = new(file);

  tmp = ob->short();

  if(ob->get() && !optn)
    err = ob->move(this_player());
  else
    err = ob->move(environment(this_player()));

  if(err < 1) {
    if(ob) ob->remove();
    if(ob) destruct(ob);
    return notify_fail("You fail.\n");
  }

  printf("You clone %s.\n", tmp);
  say(sprintf("%s pulls %s out of thin air!\n",
	      this_player()->query_cap_name(),
	      tmp));

  return 1;
}

string
help_desc()
{
  return "Clone a copy of an object from a file.";
}

string
help()
{
  return USAGE + @ENDHELP

Make a clone of the object described in <filename>.  If the object will not
load, check the files in /log/errors or the files in ~/errors.

Options:
  -n          No move to inventory.  Normally object is moved to player's 
              inventory if get() in the object returns 1.
ENDHELP;
}
