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
  string contents = read_file( file );
  if( !contents )
    {
      return sprintf("Can't find file: %s<BR>", file );
    }
	  return sprintf("%s<BR><BR><TEXTAREA NAME=\"editfile\" ROWS=35 COLS=80>%s</TEXTAREA></HTML>",
		str1, contents );
}
