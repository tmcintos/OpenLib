/*  -*- LPC -*-  */
#include <command.h>
#include <cmdline.h>

int
_main(string file, int argc, string *argv, string *flags)
{
  object ob;
  string tmp;
  boolean optn = flag(flags, "n");

  if (!file) {
    return notify_fail("usage: clone <filename>\n");
  }

  file = RESOLVE_PATH(file);
  
  if((file_size(file) < 0) && (file_size(file+".c") < 0))
    return notify_fail("clone: file does not exist.\n");
  
  ob = new(file);

  tmp = ob->short();

  if(ob->get() && !optn)
    ob->move(this_player());
  else
    ob->move(environment(this_player()));

  printf("You clone %s.\n", tmp);
  say(sprintf("%s pulls %s out of thin air!\n",
	      this_player()->query_cap_name(),
	      tmp));

  return 1;
}
