#include <dirs.h>

void
log_file(string filename, string logMsg)
{
  unguarded((: write_file, LOG_DIR "/"+filename, ctime(time())+": "+logMsg :),
            1);
}
