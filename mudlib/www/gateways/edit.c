#include <mudlib.h>
#include <daemons.h>

inherit DAEMON;

string str1 = @ENDSTR1
<HTML>
<HEAD><TITLE>Scott's Editor</TITLE></HEAD>
<HR><P>
ENDSTR1;

varargs
string
gateway(string file)
{
  string contents;

  if( !sizeof(file) )
    return str1 + "Must provide filename<BR>\n";

  contents = read_file( file );

  if( !contents )
    {
      return str1 + sprintf("Can't find file: %s<BR>", file );
    }
//  return sprintf("%s<BR><BR><TEXTAREA NAME=\"editfile\" ROWS=35 COLS=80>%s</TEXTAREA></HTML>", str1, contents );
  return sprintf("%s<BR><BR><PRE NAME=\"editfile\" ROWS=35 COLS=80>%s</PRE></HTML>", str1, contents );}
