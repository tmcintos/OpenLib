#include <dirs.h>

void
log_file(string filename, string logMsg)
{
  seteuid( geteuid(previous_object()) );

  write_file(LOG_DIR "/"+ filename,
	     ctime(time()) + ":  " + logMsg, 0);
}
