/*  -*- LPC -*-  */
// finger.c:  finger gateway for WWW
//   Hastily coded by Tim on 11/1/95
#include <mudlib.h>
#include <daemons.h>

inherit DAEMON;

// header for generic finger
string str1 = @ENDHTML
<HTML>
<HEAD><TITLE> Finger Report </TITLE></HEAD>
<BODY bgcolor=##9370DB link=#00FFFF>

<H1> Users Currently on Dysfunctional Mud: </H1>

<HR>

<UL>
ENDHTML;

// header for specific finger
string str3 = @ENDHTML
<HTML>
<HEAD><TITLE> Finger Report </TITLE></HEAD>
<BODY bgcolor=##9370DB link=#00FFFF>

<H1> Finger Report on User: </H1>

<HR>

<PRE>
ENDHTML;

// footer for all pages
string str2 = @ENDHTML
<HR>

<A HREF="/index.html">Back to Main Page</A>
 <P>
<ADDRESS> <I>Dysfunctional Mud</I>
 / <I>mud@knapp38.res.iastate.edu</I> </ADDRESS>
</BODY>
</HTML>
ENDHTML;

varargs
string
gateway(string who)
{
  string ret = "";

  if(!who) {
    object user;
    foreach(user in users())
      ret  += sprintf("<LI> %s", user->short());
    return  sprintf("%s%s</UL>%s", str1, ret, str2);
  } else {  
    ret = sprintf("%s\n%s\n</PRE>%s", str3,
		  FINGER_D->GetFinger(who), str2);
    return ret;
  }
  return "";
}
