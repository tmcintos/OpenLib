/*  -*- LPC -*-  */
// finger.c:  finger gateway for WWW
//   Hastily coded by Tim on 11/1/95
//   Rewritten somewhat by Tim 29 Aug 1996
#include <mudlib.h>
#include <daemons.h>

inherit DAEMON;

//
// header for generic finger
//
private string gen_header = @ENDHTML
<HTML>
<HEAD><TITLE>Dysfunctional Mud Finger Lookup</TITLE></HEAD>
<BODY bgcolor=##9370DB link=#00FFFF>
<H1>Users Currently on Dysfunctional Mud</H1>
<HR>
<UL>
ENDHTML;
//
// header for specific finger
//
private string spec_header = @ENDHTML
<HTML>
<HEAD><TITLE>User Information</TITLE></HEAD>
<BODY bgcolor=##9370DB link=#00FFFF>
<H1>Information for %s</H1>
<HR>
ENDHTML;
//
// footer for all pages
//
private string footer = @ENDHTML
<HR>
<H1>Enter a Username Below for Detailed Information</H1>
<FORM ACTION="/gateways/finger">
  <P><INPUT TYPE=TEXT NAME=username MAXLENGTH=12>
  <P><INPUT TYPE=RESET VALUE=Reset> <INPUT TYPE=SUBMIT VALUE=Submit>
</FORM>
<HR>
<A HREF="/index.html">Back to Main Page</A>
<ADDRESS>
<I>Dysfunctional Mud</I> /
<A HREF="mailto:mud@knapp38.res.iastate.edu">mud@knapp38.res.iastate.edu</A>
</ADDRESS>
</BODY>
</HTML>
ENDHTML;
//
// PRE: 'args' can be specified as 0 for a general finger, a username, or
//      "username=" + a username
varargs
string
gateway(string args)
{
  string who, ret = "";

  if( !args )
  {
    object user;
    foreach(user in users())
      ret  += sprintf("<LI> %s\n", user->short());
    return gen_header + ret + "</UL>\n" + footer;
  }
  else
  {
    if( sscanf(args, "username=%s", who) != 1 || !strlen(who) )
      who = args;
    who = lower_case(who);
    return sprintf(spec_header + "<PRE>%s</PRE>\n" + footer, capitalize(who),
		   FINGER_D->GetFinger(who));
  }
}
