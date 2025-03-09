/*  -*- LPC -*-  */
#ifndef _FTP_D_H
#define _FTP_D_H

#define ANONYMOUS_FTP
#define README_FILES
#define MESSAGE_FILES

#define BLOCK_SIZE 1024
#define FTP_MOTD "/adm/etc/ftpd_login"

// socket flags:
#define F_LOGGED_IN 1
#define F_BINARY    2
#define F_UPLOAD    4
#define F_ERROR     8

// macros for testing flags:  (ex: SET(sock, F_ERROR); )
#define IS_SET(x,y) (((class socket) x)->bits & y)
#define SET(x,y)    ((class socket) x)->bits |= y
#define CLR(x,y)    ((class socket) x)->bits &= ~y

// For use with eventOpenDataConn() as 'type'
#define T_ASCII   1
#define T_BINARY  2
#define T_DEFAULT 3

// Some macros for producing messages:
// NOTE that the first 2 are set up to take a string constant only.
#define CmdSuccess200(x) "200 " x " command successful.\n"
#define CmdSuccess250(x) "250 " x " command successful.\n"
#define NoSuchFile550(x) "550 "+ x +": No such file or directory.\n"
#define PermDenied550(x) "550 "+ x +": Permission denied.\n"
#define PermDenied553(x) "553 "+ x +": Permission denied.\n"
#define CmdNotUnd500(x) "500 '"+ x +"': command not understood.\n"

class socket {
  int fd;               // file descriptor
  int data_fd;          // file descriptor for the data socket
  /*
  ** bitfield flags:
  ** 0: data bit -- set (1) if this is a data socket
  ** 1: logged in -- set if user is logged in
  ** 2: default tranfer type -- 1 for BINARY, 0 for ASCII mode
  ** 3: upload or download -- 1 for UPLOAD, 0 for DOWNLOAD
  ** 4: error bit -- set if an error occurred during transfer (data sockets)
  */
  int bits;

  /*
  ** These are for interactive sockets
  */
  string username;
  mixed priv;
  string cwd;
  string remote_host;
  string data_addr;
  int data_port;
  string file;           // file being transferred (as typed)
  string filename;       // full path
  /*
  ** These are for data sockets
  */
  int pos;               // current position in transfer
}

static void eventNewConnection(int fd);
static void eventResolveCallback(string ipnum, string hostname, int key);
static void eventWriteError(int fd);
static void eventSocketClosed(int fd);
static void eventRead(int fd, mixed val);
static void eventReadData(class socket sock, mixed val);
static int  eventOpenDataConn(class socket sock, int type);
static void eventSendData(class socket sock);
static void eventShowFiles(class socket sock, string code);
static void requestUSER(class socket sock, string uname);
static void requestPASS(class socket sock, string passwd);
static void requestPORT(class socket sock, string addr);
static void requestTYPE(class socket sock, string type);
static void requestRETR(class socket sock, string filename);
static void requestSTOR(class socket sock, string filename);
static void requestXCWD(class socket sock, string pathname);
static void requestLIST(class socket sock, string pathname);
static void requestXPWD(class socket sock);
#ifdef ANONYMOUS_FTP
static string PseudoRootPath(string cur, string rel);
#endif
mixed* query_connections();

#endif /* _FTP_D_H */
