#include <command.h>

int
main()
{
  int i, size, fd;
  string *lines;

  lines = explode(dump_socket_status(), "\n");
  size = sizeof(lines);

  for(i=0; i < size; i++) {
    sscanf(lines[i], "%d %*s", fd);
    if(fd != -1)
      write(lines[i] + "\n");
  }

  return 1;
}
