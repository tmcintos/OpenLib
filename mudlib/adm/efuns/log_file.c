#include <dirs.h>

void
log_file(string filename, string logMsg)
{
  write_file(LOG_DIR "/"+ filename,
	     ctime(time()) + ":  " + logMsg, 0);
}
