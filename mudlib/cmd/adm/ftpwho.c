#include <command.h>
#include <net/daemons.h>
#include <net/ftpd.h>

int
main()
{
  int i;
  mapping *connections;

  connections = FTP_D->query_connections();

  i = sizeof(connections);

  write(sprintf("%-15s                    %-15s\n",
		"User Name", "From Address"));
  write("---------                          ------------\n");

  while(i--)
    write(sprintf("%-15s                    %-15s\n",
		  connections[i][USER_NAME], 
		  connections[i][USER_SITE]));

  return 1;
}
