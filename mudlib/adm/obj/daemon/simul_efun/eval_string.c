/*  -*- LPC -*-  */
// This should be secure because it uses the secure temporary dir, which
// is not writable by non-root types. (also giving priv 1 to those who need it)
// The code is also evaluated at the priv level of the true caller.

#include <dirs.h>

#define TFILE SECURE_TMP_DIR "/tmp_eval_file"

mixed
eval_string(string arg)
{
  string filetext =
@ENDTEXT
#include <mudlib.h>
#include <daemons.h>
#include <net/daemons.h>

mixed
eval()
{
  function f = function() { object me = this_player(); %s; };
  mixed priv = (this_interactive() ? query_privs(this_interactive()) : 0);

  return unguarded(f, priv);
}
ENDTEXT;
  
  // clean up first
  if( find_object(TFILE) )
    destruct(find_object(TFILE));

  filetext = sprintf(filetext, arg);

  // Write out the file
  unguarded((: write_file, TFILE ".c", filetext, 1 :), 1);

  return TFILE->eval();
}
