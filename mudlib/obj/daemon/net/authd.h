/*  -*- LPC -*-  */
#ifndef _AUTHD_H
#define _AUTHD_H

class _keyset {
  int conn_there;
  int conn_there_timestamp;
  int conn_here;
  int conn_here_timestamp;
}

#define keyset class _keyset

#endif /* _AUTHD_H */
