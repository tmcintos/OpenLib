#include <cmdline.h>
#include <daemons.h>

int
main(string* argv1, string* argv2)
{
  mixed* val = NEWS_D->read_post(argv2[0], to_int(argv2[1]));

  if( val )
    printf("Date %s\nThread: %s\nSubject: %s\nAuthor: %s\n%s\n",
	   ctime(val[0]), val[1], val[2], val[3], val[4]);
  else
    write("Post was deleted.\n");

  return 1;
}
