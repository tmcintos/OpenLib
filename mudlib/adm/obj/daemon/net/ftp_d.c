/*  -*- LPC -*-  */
// ftp_d.c:  LPC FTP server.
//
// Copyright (C) 1996 Tim McIntosh (tmcintos@dm.imaginary.com)
//
// This program is part of the OpenLib Mudlib distribution; it
// is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published
// by the Free Software Foundation; either version 2 of the License,
// or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// If you acquired this program as part of the OpenLib Mudlib
// distribution, you should have received a copy of the GNU
// General Public License in the file /doc/GPL; if not,
// write to the Free Software Foundation, Inc., 675 Mass Ave,
// Cambridge, MA 02139, USA.
//
// Created 1-2 Jun 1996 by Tim McIntosh
//
// Security is done by calling valid_* through unguarded() with the unguarded
// priv set to that of the user performing the action.
//
// TODO:
//  - implement a lot more of the commands (mkdir, rmdir, dele, etc.)
//  - facilities to check for valid email addresses when using anonymous ftp
//  - double check that security checking mechanisms are working

#include <daemons.h>
#include <net/network.h>
#include "ftp_d.h"

#define FTP_VERSION "0.6"

inherit TCP_SERVER;

// take out eventually
#include <net/ftp.h>

mapping connections;
mapping resolve_map;

void
create()
{
  server::create();
  connections = ([]);
  resolve_map = ([]);
  call_out("setup", 2);
}

static void
setup()
{
  eventListenSocket(PORT_FTP, STREAM);
}

static void
eventNewConnection(int fd)
{
  class socket sock = new( class socket );

  server::eventNewConnection(fd);
  connections[fd] = sock;
  sock->fd = fd;
  sock->remote_host = explode(socket_address(fd), " ")[0];
  resolve_map[resolve(sock->remote_host, "eventResolveCallback")] = fd;

  eventWrite(fd,
	     sprintf("220- %s FTP server (Version ul-%s (LPC)) ready.\n"
#ifdef ANONYMOUS_FTP
		     "220  Please login as yourself or 'anonymous'.\n",
#else
		     "220  Please login as yourself.\n",
#endif /* ANONYMOUS_FTP */
		     mud_name(), FTP_VERSION));
}

static void
eventResolveCallback(string hostname, string ipnum, int key)
{
  class socket sock = connections[resolve_map[key]];

  if( !sock || !hostname ) return;

  sock->remote_host = hostname;
  map_delete(resolve_map, key);
}

static void
eventWriteError(int fd)
{
  class socket sock = connections[fd];

  if( sock ) SET(sock, F_ERROR);
}

static void
eventSocketClosed(int fd)
{
  class socket sock = connections[fd];

  if( sock && fd == sock->data_fd && !IS_SET(sock, F_ERROR) )
  {
    /*
    ** Data socket closed error free.
    */
    eventWrite(sock->fd, "226 Transfer Complete.\n");
  }
  map_delete(connections, fd);
}

// This macro checks for the proper number of arguments to a command
// syntax: CHECK_INSTR(minimum_number_args,maximum_number_args)

#define CHECK_INSTR(x,y) if( sizeof(instruction) < (1 + x) || \
			     sizeof(instruction) > (1 + y) ) break;

#define CHECK_LOGGED_IN() if( !IS_SET(connections[fd], F_LOGGED_IN) ) \
                          return eventWrite(fd, "530 Please login with " \
				       "USER and PASS.\n"); \

static void
eventRead(int fd, mixed val)
{
  class socket sock = connections[fd];
  string* instruction;

  if( fd == sock->data_fd ) return eventReadData(sock, val);/* a data socket */

  val = val[0..<3];  // strip off cr and lf

  instruction = explode(val, " ");
  if( !sizeof(instruction) ) return;

  instruction[0] = upper_case(instruction[0]);

  /* Had problems w/special characters being sent with ABOR, so added this;
     don't know what the spec says about it. */

  while( instruction[0][0] < 'A' || instruction[0][0] > 'Z' )
    instruction[0] = instruction[0][1..];

  switch( instruction[0] ) {
  case "USER":
    CHECK_INSTR(1,1);
    requestUSER(sock, instruction[1]);
    return;
  case "PASS":
    CHECK_INSTR(1,1);
    requestPASS(sock, instruction[1]);
    return;
  case "QUIT":
    CHECK_INSTR(0,0);
    map_delete(connections, fd);
    eventWrite(fd, "221 Goodbye.\n", TRUE);
    return;
  case "PORT":
    CHECK_INSTR(1,1);
    requestPORT(sock, instruction[1]);
    return;
  case "TYPE":
    CHECK_INSTR(1,1);
    requestTYPE(sock, instruction[1]);
    return;
  case "RETR":
    CHECK_INSTR(1,1);
    CHECK_LOGGED_IN();
    requestRETR(sock, instruction[1]);
    return;
  case "STOR":
    CHECK_INSTR(1,1);
    CHECK_LOGGED_IN();
    requestSTOR(sock, instruction[1]);
    return;
  case "REST":
    CHECK_INSTR(1,1);
    sock->pos = to_int(instruction[1]);
    eventWrite(fd, "350 Restarting at " + instruction[1] + ".\n");
    return;
  case "ABOR":
    CHECK_INSTR(0,0);
    socket_close(sock->data_fd);
    eventWrite(fd, "225 ABOR command successful.\n");
    return;
  case "CWD": case "XCWD":
    CHECK_INSTR(0,1);
    CHECK_LOGGED_IN();
    if( sizeof(instruction) > 1 )
      requestXCWD(sock, instruction[1]);
    else
      requestXCWD(sock, 0);
    return;
  case "LIST":
    CHECK_INSTR(0,1);
    CHECK_LOGGED_IN();
    if( sizeof(instruction) > 1 )
      requestLIST(sock, instruction[1]);
    else
      requestLIST(sock, 0);
    return;
  case "NLST":
    CHECK_INSTR(0,1);
    CHECK_LOGGED_IN();
    if( sizeof(instruction) > 1 )
      requestNLST(sock, instruction[1]);
    else
      requestNLST(sock, 0);
    return;
  case "SYST":
    CHECK_INSTR(0,0);
    eventWrite(fd, sprintf("215 MUD Type: LP Version: %s\n", __VERSION__));
    return;
  case "NOOP":
    CHECK_INSTR(0,0);
    eventWrite(fd, CmdSuccess200("NOOP"));
    return;
  case "PWD": case "XPWD":
    CHECK_INSTR(0,0);
    CHECK_LOGGED_IN();
    requestXPWD(sock);
    return;
  default:
    if( cmdtab[lower_case(instruction[0])] )
    {
      eventWrite(fd, sprintf("502 %s command not implemented.\n",
			     instruction[0]));
      return;
    }
  } /* switch() */

  /*
   * If no match is found or the wrong # of args is detected, it falls
   * through to this.
   */
  eventWrite(fd, CmdNotUnd500(val));
}

static void
eventReadData(class socket sock, mixed val)
{
  if( IS_SET(sock, F_BINARY) )                    /* BINARY mode */
    write_buffer(sock->filename, sock->pos, val);
  else                                            /* ASCII mode */
  {
    val = replace_string(val, "\r\n", "\n");    // remove carriage returns
    write_bytes(sock->filename, sock->pos, val);
  }

  sock->pos += sizeof(val);
}  /* eventReadData() */

static int
eventOpenDataConn(class socket sock, int type)
/*
** Post: setup the data socket for 'sock' and open the
**       connection.  Returns 1 on success; 0 on fail.
*/
{
  string stype;
  int err, fd, size;

  if( type == T_DEFAULT )
  {
    if( IS_SET(sock, F_BINARY) )
      type = T_BINARY;
    else
      type = T_ASCII;
  }

  switch( type )
  {
  case T_ASCII:
    stype = "ASCII";
    err = eventCreateSocket(sock->data_addr, sock->data_port, STREAM);
    break;
  case T_BINARY:
    stype = "BINARY";
    err = eventCreateSocket(sock->data_addr, sock->data_port, STREAM_BINARY);
    break;
  default:
    /*
     * should not happen
     */
    eventWrite(sock->fd,
	       "425 Can't build data connection: server error (bad type).\n");
    return 0;
  }

  if( err < 0 )
  {
    eventWrite(sock->fd, sprintf("425 Can't build data connection: %s.\n",
				 socket_error(err)));
    return 0;
  }
  fd = err;

  /*
   * Initialize the data socket values in 'sock'
   */
  connections[fd] = sock;
  sock->data_fd = fd;
  size = (sock->filename[0] != '/' ? sizeof(sock->filename) :
                                     file_size(sock->filename));

  eventWrite(sock->fd, sprintf("150 Opening %s mode data connection "
			       "for %s.", stype, sock->file));

  if( !IS_SET(sock, F_UPLOAD) )
    eventWrite(sock->fd, sprintf(" (%d bytes)\n", size - sock->pos));
  else
    eventWrite(sock->fd, "\n");

  return 1;
}  /* eventOpenDataConn() */

static void
eventSendData(class socket sock)
/*
** Post: If sock->filename does not begin with '/', it is sent as the data;
**       otherwise the data is read from the file sock->filename.
*/
{
  int fd = sock->data_fd;
  function read_func;
  mixed tmp;
  
  if( sock->filename[0] != '/' )
  {
    string data = replace_string(sock->filename, "\n", "\r\n"); // insert cr's
    int i;

    for(i = 0; i < sizeof(data) - BLOCK_SIZE; i += BLOCK_SIZE)
      eventWrite(fd, data[i..i+BLOCK_SIZE-1]);

    eventWrite(fd, data[i..i+BLOCK_SIZE-1], TRUE);
    return;
  }

  if( IS_SET(sock, F_BINARY) )      /* BINARY mode */
    read_func = (: read_buffer :);
  else                              /* ASCII mode */
    read_func = function(string fname, int start, int size)
                {
		  string ret = read_bytes(fname, start, size);

		  if( ret )
		    return replace_string(ret, "\n", "\r\n");  // insert cr's
		  else
		    return 0;
		};

  tmp = evaluate(read_func, sock->filename, sock->pos, BLOCK_SIZE);

  if( !tmp ) socket_close(fd);

  while( tmp )
  {
    mixed next;

    sock->pos += BLOCK_SIZE;
    next = evaluate(read_func, sock->filename, sock->pos, BLOCK_SIZE);
     
    eventWrite(fd, tmp, (next ? FALSE : TRUE));
    tmp = next;
  }
}  /* eventSendData() */

static void eventShowFiles(class socket sock, string code)
{
#if defined(README_FILES) || defined(MESSAGE_FILES)
  string tmp;
  string dir = sock->cwd;

  dir += (dir[<1] == '/' ? "" : "/");
#endif

#ifdef MESSAGE_FILES
  tmp = dir + ".message";

  if( tmp = read_file(tmp) )
    eventWrite(sock->fd, code + "-"
	       + replace_string(tmp, "\n", "\n" + code + "-") + "\n");
	       
#endif /* MESSAGE_FILES */

#ifdef README_FILES
  tmp = dir + "README";

  if( file_size(tmp) > -1 )
  {
    int* info = stat(tmp);
    eventWrite(sock->fd,
	       sprintf("%s-Please read the file README\n"
		       "%s-  it was last modified on %s - %d days ago.\n",
		       code,code, ctime(info[1]), (time() - info[1]) / 86400));
  }
#endif /* README_FILES */
} /*  eventShowFiles() */

static void
requestUSER(class socket sock, string uname)
{
  int fd = sock->fd;

  sock->username = uname;
#ifdef ANONYMOUS_FTP
  if( uname == "anonymous" )
    eventWrite(fd, sprintf("331 Guest login ok, send your complete email "
			   "address as password.\n"));
  else
#endif
  eventWrite(fd, sprintf("331 Password required for %s.\n", uname));
}  /* requestUSER() */

static void
requestPASS(class socket sock, string passwd)
{
  string text;
  int fd = sock->fd;

  if( !sock->username )
  {
    eventWrite(fd, "503 Login with USER first.\n");
    return;
  }

#ifdef ANONYMOUS_FTP
  if( sock->username != "anonymous" &&
      !LOGIN_D->check_passwd(sock->username, passwd) )
#else
  if( !LOGIN_D->check_passwd(sock->username, passwd) )
#endif
  {
    sock->username = 0;
    eventWrite(fd, "530 Login incorrect.\n");
  }
  else
  {
#ifdef ANONYMOUS_FTP
    if( sock->username == "anonymous" )
      if( strsrch(passwd, '@') < 0 )
      {
	eventWrite(fd, sprintf("230-The response '%s' is not valid\n"
			       "230-Next time please use your email address "
			       "as your password.\n"
			       "230-  for example: joe@%s\n",
			       passwd, sock->remote_host));
      }
      else
      {
	eventWrite(fd, sprintf("230-Welcome to %s, user at %s!\n",
			       mud_name(), sock->remote_host));
      }
#endif
    SET(sock, F_LOGGED_IN);
    /*
     * Setup Privs
     */
    if( admin_exists(sock->username) )
      sock->priv = 1;
    else if( wizard_exists(sock->username) )
      sock->priv = sock->username;
    else
      sock->priv = 0;

    if( wizard_exists(sock->username) )
      sock->cwd = user_cwd(sock->username);
    else
      sock->cwd = FTP_DIR;

    if( text = read_file(FTP_MOTD) )
      eventWrite(fd, "230-" + replace_string(text, "\n", "\n230-") + "\n");

    eventShowFiles(sock, "230");
#ifdef ANONYMOUS_FTP
    if( sock->username == "anonymous" )
    {
      eventWrite(fd,
		 sprintf("230 Guest login ok, access restrictions apply.\n"));
      sock->cwd = FTP_DIR;
    }
    else
#endif /* ANONYMOUS_FTP */
    eventWrite(fd, sprintf("230 User %s logged in.\n", sock->username));
  }
}  /* requestPASS() */

static void
requestPORT(class socket sock, string addr)
{
  string* tmp = explode(addr, ",");

  if( sizeof(tmp) != 6 )
  {
    eventWrite(sock->fd, CmdNotUnd500("PORT " + addr));
    return;
  }
  sock->data_addr = implode(tmp[0..3], ".");
  sock->data_port = (to_int(tmp[4]) << 8) + to_int(tmp[5]);
  sock->pos = 0;  /* Reset position -- should this be done here ? */
  eventWrite(sock->fd, CmdSuccess200("PORT"));
}  /* requestPORT() */
  
static void
requestTYPE(class socket sock, string type)
{
  switch( type )
  {
  case "A":
    CLR(sock, F_BINARY);
    break;
  case "I":
    SET(sock, F_BINARY);
    break;
  default:
    eventWrite(sock->fd, CmdNotUnd500("TYPE " + type));
    return;
  }
  eventWrite(sock->fd, sprintf("200 Type set to %s.\n", type));
}  /* requestTYPE() */

static void
requestRETR(class socket sock, string filename)
{
  sock->file = filename;
#ifdef ANONYMOUS_FTP
  if( sock->username == "anonymous" )
    filename = PseudoRootPath(sock->cwd, filename);
  else
#endif
  filename = absolute_path(sock->cwd, filename);

  if( !unguarded((: master()->valid_read($(filename), 0, 0) :), sock->priv) )
  {
    eventWrite(sock->fd, PermDenied550(sock->file));
    return;
  }

  switch( file_size(filename) )
  {
  case -1:
    eventWrite(sock->fd, NoSuchFile550(sock->file));
    break;
  case -2:
    eventWrite(sock->fd, "550 "+ sock->file +": Not a plain file.\n");
    break;
  default:
    sock->filename = filename;
    CLR(sock, F_UPLOAD); /* Download -- should already be clear, but oh-well */
    if( !eventOpenDataConn(sock, T_DEFAULT) ) return;
    eventSendData(sock);
  }
}  /* requestRETR() */

static void
requestSTOR(class socket sock, string filename)
{
  sock->file = filename;
#ifdef ANONYMOUS_FTP
  if( sock->username == "anonymous" )
    filename = PseudoRootPath(sock->cwd, filename);
  else
#endif
  filename = absolute_path(sock->cwd, filename);

  if( !unguarded((: master()->valid_write($(filename), 0, 0) :), sock->priv) )
  {
    eventWrite(sock->fd, PermDenied553(sock->file));
    return;
  }

  switch( file_size(filename) )
  {
  case -1:
    break;
  case -2:
    eventWrite(sock->fd, "553 "+ sock->file +": Is a directory.\n");
    return;
  default:
    rm(filename);
  }

  if( !directory_exists(filename[0..strsrch(filename, '/', -1)-1]) )
  {
    eventWrite(sock->fd, "553 Could not determine cwdir: No such file "
	       "or directory.\n");
    return;
  }

  sock->filename = filename;
  SET(sock, F_UPLOAD);
  if( !eventOpenDataConn(sock, T_DEFAULT) ) return;
}  /* requestSTOR() */

static void
requestXCWD(class socket sock, string pathname)
{
  if( pathname )
  {
    string new_cwd;
#ifdef ANONYMOUS_FTP
    if( sock->username == "anonymous" )
      new_cwd = PseudoRootPath(sock->cwd, pathname);
    else
#endif
    new_cwd = absolute_path(sock->cwd, pathname);

    if( !unguarded((: master()->valid_read($(new_cwd), 0, 0) :), sock->priv) )
    {
      eventWrite(sock->fd, PermDenied550(pathname));
      return;
    }

    if( file_size(new_cwd) != -2 )
    {
      eventWrite(sock->fd, NoSuchFile550(pathname));
      return;
    }
    else sock->cwd = new_cwd;
  }
  else
  {
#ifdef ANONYMOUS_FTP
    if( sock->username == "anonymous" )
      sock->cwd = FTP_DIR;
    else
#endif
    sock->cwd = user_cwd(sock->username);
  }
  eventShowFiles(sock, "250");
  eventWrite(sock->fd, CmdSuccess250("CWD"));
}  /* requestXCWD() */

/*
 * This code originally from /adm/cmd/wiz/ls.c
 */
private string
long_list(string dir, mixed* file)
{
  
  string ct = ctime(file[2]);
  return sprintf("%:-10s %-10s %6s %s %s\n",
	 sprint_priv(SECURITY_D->get_file_protection(dir + file[0], 0)),
	 sprint_priv(SECURITY_D->get_file_protection(dir + file[0], 1)),
		 (file[1] == - 2 ? "<DIR>" : "" + file[1]),
		 ct[4..15] + ct[19..],
		 file[0]);
}

static void
requestLIST(class socket sock, string pathname)
{
  /**** Originally from the 'file' service implementation of I3 ****/
  string* files;
  string dir;

  sock->file = "/bin/ls";

  if( pathname )
  {
#ifdef ANONYMOUS_FTP
    if( sock->username == "anonymous" )
      dir = PseudoRootPath(sock->cwd, pathname);
    else
#endif
    dir = absolute_path("/", pathname);
  }
  else
  {
    dir = sock->cwd;
  }

  if( !unguarded((: master()->valid_read($(dir), 0, 0) :), sock->priv) )
  {
    eventWrite(sock->fd, PermDenied550(pathname));
    return;
  }

  switch( file_size(dir) )
  {
  case -1:
    files = 0;
    break;
  case -2:
    files = get_dir(dir + (dir[<1] == '/' ? "*" : "/*"), -1);
    break;
  default:
    files = ({ get_dir(dir, -1) });
  }

  if( sizeof(files) )
  {
    files = map(files, (: long_list, (dir[<1] == '/' ? dir : dir+"/") :));
    sock->filename = implode(files, "");
  }
  else
    sock->filename = "No files found.\n";

  if( !eventOpenDataConn(sock, T_ASCII) ) return;
  eventSendData(sock);
}  /* requestLIST() */

static void
requestNLST(class socket sock, string pathname)
{
  /**** Originally from the 'file' service implementation of I3 ****/
  string* files;
  string dir;

  sock->file = "/bin/nlst";

  if( pathname )
  {
#ifdef ANONYMOUS_FTP
    if( sock->username == "anonymous" )
      dir = PseudoRootPath(sock->cwd, pathname);
    else
#endif
    dir = absolute_path("/", pathname);
  }
  else
  {
    dir = sock->cwd;
  }

  if( !unguarded((: master()->valid_read($(dir), 0, 0) :), sock->priv) )
  {
    eventWrite(sock->fd, PermDenied550(pathname));
    return;
  }

  switch( file_size(dir) )
  {
  case -1:
    files = 0;
    break;
  case -2:
    files = get_dir(dir + (dir[<1] == '/' ? "" : "/"));
    break;
  default:
    files = ({ get_dir(dir) });
  }

  if( sizeof(files) )
    sock->filename = implode(files, "\n") + "\n";
  else
    sock->filename = "";

  if( !eventOpenDataConn(sock, T_ASCII) ) return;
  eventSendData(sock);
}  /* requestNLST() */

static void
requestXPWD(class socket sock)
{
  string cwd = sock->cwd;

#ifdef ANONYMOUS_FTP
  if( sock->username == "anonymous" )
  {
    cwd = cwd[sizeof(FTP_DIR)..];
    if( cwd[0] != '/' ) cwd = "/" + cwd;
  }
#endif

    eventWrite(sock->fd, sprintf("257 \"%s\" is current directory.\n", cwd));
}

#ifdef ANONYMOUS_FTP
/**** Originally from the 'file' service implementation of I3 ****/

static string
PseudoRootPath(string cur, string rel)
{
  if( rel[0] == '/' ) rel = FTP_DIR + rel;
  rel = absolute_path(cur, rel);
  if( !strsrch(rel, FTP_DIR) )
    return rel;                     // FTP_DIR is a prefix of rel
  else
    return FTP_DIR + rel;                 // tried to go beyond the root
}
#endif /* ANONYMOUS_FTP */

mixed*
query_connections()
{
  mixed* ret = ({});
  foreach(class socket sock in distinct_array(values(connections)))
    ret += ({ ({ sock->username, sock->priv, sock->remote_host }) });

  return ret;
}
