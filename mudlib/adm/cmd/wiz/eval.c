#include <command.h>
#include <dirs.h>

#define TFILE TMP_DIR "/tmp_eval_file"

int
main(string arg)
{
  if(arg == "" || !arg)
    return notify_fail("usage: eval <lpc code>\n");

  /* clean up first */
  if (file_size(TFILE ".c") != -1)
    rm (TFILE ".c");
  if (find_object(TFILE))
    destruct(find_object(TFILE));

  write_file(TFILE ".c",
	     "#include <mudlib.h>\n"
	     "#include <daemons.h>\n"
	     "mixed eval() { "+arg+"; }\n");
  printf("Result = %O\n", TFILE->eval());
  return 1;
}
