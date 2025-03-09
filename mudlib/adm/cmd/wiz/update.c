/*  -*- LPC -*-  */
// update.c:  -n option is for no-load (doesn't reload file)
//            -d option is to update all files in a directory

#include <command.h>
#include <cmdline.h>

#define USAGE "usage: update [-nArRs] [-nds] <filename>\n"

varargs int update_file(string file, boolean reload, boolean silent);

int
_main(string *argv, string *argv2)
{
  string *files, file;
  int i, reload, size, hush;

  if (!sizeof(argv2))
    return notify_fail(USAGE);

  file = RESOLVE_PATH(argv2[0]);              // first real argument

  reload = !flag("n");
  hush = flag("s");

  if(!flag("d")) {

// This will destruct all cloned copies of this object also
// prob move this to the dest command later...

    if(flag("A")) {
      object ob;

      foreach(ob in children(file)) {
	write("destructing: "+ file_name(ob) +":");

	if(!destruct(ob))
	  write(" destruct failed.\n");
	else
	  write(" ok.\n");
      }
    }

// Recursive Updating:  big R is deep inherit list

    if((flag("r") || flag("R"))) {
      object ob;
      
      if(!(ob = find_object(file)))
	return notify_fail("update: object must be loaded for "
			   "recursive update.\n");

      if(flag("r"))
	files = shallow_inherit_list(find_object(file));
      else
	files = deep_inherit_list(find_object(file));
      
      for(i=sizeof(files)-1; i >= 0; i--) {
	if(!hush)
	  write("update: " + files[i] +": ");
	update_file(files[i], TRUE, hush);
      }
    }

// Actual update of indicated object
    if(!hush)
      write("update: ");
    update_file(file, reload, hush);
    return 1;

// Update all files in a directory
  } else /* if 'd' flag was passed */ {
    if(file_size(file) != -2)
      return notify_fail(sprintf("%s: directory doesn't exist.\n", file));

    if(file[strlen(file) - 1] != '/')
      file += "/";
    files = get_dir(file + "*.c");
    size = sizeof(files);
				      
    write(file + ":\n");
    for(i=0;i < size;i++) {
      if(!hush)
	write(files[i] + ": ");
      update_file(file + files[i], reload, hush);
    }
  }
  return 1;
}

varargs
int
update_file(string file, boolean reload, boolean silent)
// pre:  file must exist
// modifies:  object is destructed if already loaded
// post:  object in 'file' is updated; returns 1 if re-loaded, 0 if not
{
  mixed err;
  object obj;
  
  if((file_size(file) < 0) && (file_size(file+".c") < 0)) {
    printf("update: %s does not exist.\n", file);
    return 0;
  }

  if (obj = find_object(file)) {
    if(!silent && obj == this_object())      // For updating this command :)
      write("This command will be reloaded at next reference.\n");
    efun::destruct(obj);                     // destruct the object first
  } else if(!silent) {
    write("object not loaded...");
  }

  if(reload) {
    err = catch(call_other(file, "???"));

    if(!err) {
      if(!silent)
	write("object re-loaded.\n");
      return 1;
    } else {
      if(!silent)
	write("failed to re-load object.\n");
      return 0;
    }
  } else {     // if(!reload)
    if(!silent)
      write("object will be reloaded at next reference.\n");
    return 0;
  }

  return 0;    // should not get here!
}

string
help_desc()
{
  return "Reload the main copy of an object in memory.";
}

string
help()
{
  return USAGE + @ENDHELP

Reload the main copy of the object defined in file <filename>.

Options:

  -A          Destruct all cloned copies of the object before updating.
              NOTE: this will probably be moved to the 'dest' command.

  -d          Update all files in the directory <filename>.

  -n          No-reload.  Object will be reloaded at next reference.

  -r,R        Recursive updating.  -r updates all objects returned by
              shallow_inherit_list(), while -R updates all objects returned
              by deep_inherit_list().

  -s          Do updating (semi-) silently.
ENDHELP;
}
