#include <command.h>
#include <net/socket.h>

int
main(string arg)
{
     mixed* info = socket_status(to_int(arg));
     string line;

     if( !info ) return notify_fail("No socket with that descriptor\n");

     switch(info[0]) {
	case CLOSED:
	  line = "Closed    ";
	  break;
	case FLUSHING:
	  line = "Flushing  ";
	  break;
	case UNBOUND:
	  line = "Unbound   ";
	  break;
	case BOUND:
	  line = "Bound     ";
	  break;
	case LISTEN:
	  line = "Listening ";
	  break;
	case DATA_XFER:
	  line = "Data Xfer ";
	  break;
	default:
	  line = "Unknown   ";
     }

     switch(info[1]) {
	case MUD:
	  line += "Mud Mode   ";
	  break;
	case STREAM:
	  line += "Stream     ";
	  break;
	case DATAGRAM:
	  line += "Datagram   ";
	  break;
	case STREAM_BINARY:
	  line += "Stream/B   ";
	  break;
	case DATAGRAM_BINARY:
	  line += "Datagram/B ";
	  break;
	default:
	  line += "Unknown    ";
     }

     line += info[2] + "  " + info[3] + "\n";

     write(line);
     return 1;
}
