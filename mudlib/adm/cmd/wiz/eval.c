/*  -*- LPC -*-  */
// This should be secure because it uses the secure temporary dir, which
// is not writable by non-root types.  The object created from the temporary
// file also sets it's privs to that of the caller. --Tim

#include <command.h>
#include <dirs.h>

#define TFILE SECURE_TMP_DIR "/tmp_eval_file"

int
main(string arg)
{
  string filetext;
  
  if(arg == "" || !arg)
    return notify_fail("usage: eval <lpc code>\n");

  /* clean up first */
  if (find_object(TFILE))
    destruct(find_object(TFILE));

  filetext = sprintf("#include <mudlib.h>\n"
		     "#include <daemons.h>\n"
		     "#include <net/daemons.h>\n"

		     "mixed eval() { \n"
		     "  object me = this_player();\n"
		     "  unguarded((: set_privs(this_object(),"
		     " query_privs(this_interactive())) :), 1);\n"
		     "  %s; }\n", arg);
  
  unguarded((: write_file(TFILE ".c", $(filetext), 1) :),
	    1);
  
  printf("Result = %O\n", TFILE->eval());
  return 1;
}
