/*  -*- LPC -*-  */
// This should be secure because it uses the secure temporary dir, which
// is not writable by non-root types. (also giving priv 1 to those who need it)
// The code is also evaluated at the priv level of the true caller.

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

  filetext =
@ENDTEXT
#include <mudlib.h>
#include <daemons.h>
#include <net/daemons.h>

mixed
eval()
{
  function f = function() { object me = this_player(); %s; };

  return unguarded(f, query_privs(this_interactive()));
}
ENDTEXT;

  filetext = sprintf(filetext, arg);
  
  unguarded((: write_file(TFILE ".c", $(filetext), 1) :),
	    1);

  printf("Result = %O\n", TFILE->eval());
  return 1;
}
