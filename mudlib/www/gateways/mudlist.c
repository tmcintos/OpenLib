/*  -*- LPC -*-  */
// mudlist.c:  Mudlist Gateway to WWW
//
// Written by Tim 29 Aug 1996
//
#include <mudlib.h>
#include <net/daemons.h>

//
// Some strings to make things simpler:
//
string header = @ENDHTML
<HTML>
<HEAD><TITLE>Dysfunctional Mud's Dynamic MUDlist</TITLE></HEAD>
<BODY bgcolor=#D9D919>
<H1>Running I3 MUDs Known to Dysfunctional Mud:</H1>
<HR>
<H2>INDEX:</H2>
ENDHTML;

string long_format = @ENDHTML
<H3><A NAME="%s" HREF="telnet://%s:%i">%s</A> [%s %i]</H3>
<I>Mudlib:</I> %s, <I>Base Mudlib:</I> %s<BR>
<I>Driver:</I> %s<BR>
<I>Status:</I> %s<BR>
<I>Admin Email:</I> %s<BR>
<I>Services:</I> %s<BR>
ENDHTML;

string footer = @ENDHTML
<HR>
<A HREF="/index.html">Back to Main Page</A>
<ADDRESS>
Dysfunctional Mud /
<A HREF="mailto:%s">%s</A>
</ADDRESS>
</BODY>
</HTML>
ENDHTML;

void
create()
{
  footer = sprintf(footer, ADMIN_EMAIL, ADMIN_EMAIL);
}

//
// Now the gateway function: (no one said this would be pretty)
//
string
gateway()
{
  string index = "";
  string long = "";
  string* muds;
  //
  // Get the mudlist mapping...
  //
  mapping mudlist = INTERMUD_D->GetMudList();
  //
  // Pick out muds which are UP currently
  //
  muds = keys(filter_mapping(mudlist, (: $2[0] == -1 :)));
  //
  // Ok, let's put together the output...
  //
  foreach(string mud in sort_array(muds, 0))
  {
    mixed* info = mudlist[mud];
    int tmp;
    
    index += sprintf("<LI><A HREF=\"#%s\">%s</A> (%s)\n", mud, mud, info[8]);

    long += sprintf(long_format, mud, info[1], info[2], mud, info[1], info[2],
		    info[5], info[6], info[7], info[9], info[10],
		    implode(keys(filter_mapping(info[11], (: $2==1 :))),", "));

    if( tmp = info[11]["http"] )
    {
      string href = sprintf("http://%s:%i/", info[1], tmp);
      long += sprintf("<I>WWW: </I><A HREF=\"%s\">%s</A><BR>\n", href, href);
    }

    if( tmp = info[11]["ftp"] )
    {
      string href = sprintf("ftp://%s:%i/", info[1], tmp);
      long += sprintf("<I>FTP: </I><A HREF=\"%s\">%s</A><BR>\n", href, href);
    }

    if( tmp = info[11]["smtp"] )
      long += sprintf("<I>SMTP port: </I>%i<BR>", tmp);

    if( tmp = info[11]["nntp"] )
      long += sprintf("<I>NNTP port: </I>%i<BR>", tmp);

    if( tmp = info[11]["rcp"] )
      long += sprintf("<I>RCP port: </I>%i<BR>", tmp);

    if( tmp = info[11]["amcp"] )
      long += sprintf("AMCP version %s<BR>\n", tmp);
  }

  return header + "<OL>\n" + index + "</OL>\n<HR>\n"
    "<H2>Detailed MUD Information:</H2>\n" + long + footer;
}
