/*  Hey emacs! This is written in -*- LPC -*-  */
/* /adm/obj/efuns/termcap.c
 *
 * © Copyright VikingMud 1993, 1994
 * Contributors: Stig Sæther Bakken (Auronthas)
 *
 * Special functions for generating terminal specific character sequences.
 *
 * Auronthas 15. Jan 1992
 *
 * 10/12/95     Tim: Changed %^END%^ to %^RESET%^ to comply with the standard
 *              in the mud community.
 * 11/14/95     Tim: further Dysfunctional-ized by making the ranges [x..y]
 *              compliant with the new MudOS standards
 * 01/22/96     Tim: added string* query_supported_termtypes()
 * 02/17/96     Tim: fixed to user copy(), changed to a daemon
 * 04/16/96     Tim: fixed to accomodate change to REVERSIBLE_EXPLODE_STRING
 */

/*
 *  W A R N I N G ! ! ! !
 *
 * You should NOT view this file from within the mud or in an editor
 * you are not sure if will quote escape characters (emacs works).
 * This file is so full of escape characters that your terminal will
 * most probably be seriously messed up from viewing it.
 *
 * The following blank lines are for your protection only.
 */
































#define ESC "%^"

mapping start_end =
([
  "UNDERLINE":"RESET",
  "BOLD":"RESET",
  "INVERSE":"RESET",
  "BLINK":"RESET",
  ]);

mapping termcap_entries =
([
  "dumb":			/* Dumb terminal */
  ([
    "CLS":"\n",
    "RESET":""
    ]),
  "vt100":
  ([
    "BELL":"",		/* audible signal (bell) */
    "CLS":"[;H[2J",		/* clear screen and home cursor */
    "BOLD":"[1m",		/* turn on bold (extra bright) attribute */
    "INVERSE":"[7m",		/* turn on reverse-video attribute */
    "BLINK":"[5m",		/* turn on blinking attribute */
    "RESET":"[m",		/* turn off all attributes */
    "TAB":"	",		/* move cursor to next hardware tab stop */
    "UNDERLINE":"[4m",	/* underline character overstrikes */
    "ESC":ESC,		/* Our escape character (so we can quote it) */
    ]),
  "vt102":  /* Tim--I'm not sure about this but it seems to work */
  ([
    "BELL":"",		/* audible signal (bell) */
    "CLS":"[;H[2J",		/* clear screen and home cursor */
    "BOLD":"[1m",		/* turn on bold (extra bright) attribute */
    "INVERSE":"[7m",		/* turn on reverse-video attribute */
    "BLINK":"[5m",		/* turn on blinking attribute */
    "RESET":"[m",		/* turn off all attributes */
    "TAB":"	",		/* move cursor to next hardware tab stop */
    "UNDERLINE":"[4m",	/* underline character overstrikes */
    "BLACK":"[30m",
    "RED":"[31m",
    "GREEN":"[32m",
    "YELLOW":"[33m",
    "BLUE":"[34m",
    "MAGENTA":"[35m",
    "CYAN":"[36m",
    "GREY":"[37m",
    "WHITE":"[37m",

    "L_RED":"[1;31m",
    "L_GREEN":"[1;32m",
    "L_YELLOW":"[1;33m",
    "L_BLUE":"[1;34m",
    "L_MAGENTA":"[1;35m",
    "L_CYAN":"[1;36m",

    "B_BLACK":"[40m",
    "B_RED":"[41m",
    "B_GREEN":"[42m",
    "B_YELLOW":"[43m",
    "B_BLUE":"[44m",
    "B_MAGENTA":"[45m",
    "B_CYAN":"[46m",
    "B_GREY":"[47m",
    "B_WHITE":"[47m",
    "ESC":ESC,		/* Our escape character (so we can quote it) */
    ]),
  "ansi" :
  ([
    "CLS":"[;H[2J",		/* clear screen and home cursor */
    "BOLD":"[1m",		/* turn on bold (extra bright) attribute */
    "INVERSE":"[7m",		/* turn on reverse-video attribute */
    "BLINK":"[5m",		/* turn on blinking attribute */
    "RESET":"[m",		/* turn off all attributes */
    "TAB":"	",		/* move cursor to next hardware tab stop */
    "UNDERLINE":"[4m",	/* underline character overstrikes */
    "BLACK":"[30m",
    "RED":"[31m",
    "GREEN":"[32m",
    "YELLOW":"[33m",
    "BLUE":"[34m",
    "MAGENTA":"[35m",
    "CYAN":"[36m",
    "GREY":"[37m",
    "WHITE":"[37m",

    "L_RED":"[1;31m",
    "L_GREEN":"[1;32m",
    "L_YELLOW":"[1;33m",
    "L_BLUE":"[1;34m",
    "L_MAGENTA":"[1;35m",
    "L_CYAN":"[1;36m",

    "B_BLACK":"[40m",
    "B_RED":"[41m",
    "B_GREEN":"[42m",
    "B_YELLOW":"[43m",
    "B_BLUE":"[44m",
    "B_MAGENTA":"[45m",
    "B_CYAN":"[46m",
    "B_GREY":"[47m",
    "B_WHITE":"[47m",
    "ESC":ESC,		/* Our escape character (so we can quote it) */
    ]),
  "debug" :
  ([
    "BOLD":"[BOLD]",		/* turn on bold (extra bright) attribute */
    "INVERSE":"[INVERSE]",		/* turn on reverse-video attribute */
    "BLINK":"[BLINK]",		/* turn on blinking attribute */
    "RESET":"[RESET]",		/* turn off all attributes */
    "TAB":"[TAB]",		/* move cursor to next hardware tab stop */
    "UNDERLINE":"[UNDERLINE]",	/* underline character overstrikes */
    "BLACK":"[BLACK]",
    "RED":"[RED]",
    "GREEN":"[GREEN]",
    "YELLOW":"[YELLOW]",
    "BLUE":"[BLUE]",
    "MAGENTA":"[MAGENTA]",
    "CYAN":"[CYAN]",
    "GREY":"[GREY]",
    "WHITE":"[WHITE]",

    "L_RED":"[L_RED]",
    "L_GREEN":"[L_GREEN]",
    "L_YELLOW":"[L_YELLOW]",
    "L_BLUE":"L_BLUE]",
    "L_MAGENTA":"[L_MAGENTA]",
    "L_CYAN":"[L_CYAN]",

    "B_BLACK":"[B_BLACK]",
    "B_RED":"[B_RED]",
    "B_GREEN":"[B_GREEN]",
    "B_YELLOW":"[B_YELLOW]",
    "B_BLUE":"[B_BLUE]",
    "B_MAGENTA":"[B_MAGENTA]",
    "B_CYAN":"[B_CYAN]",
    "B_GREY":"[B_GREY]",
    "B_WHITE":"[B_WHITE]",
    "ESC":"[ESC]",
    ]),
  "xterm":
  ([
    "BELL":"",		/* audible signal (bell) */
    "CLS":"[;H[2J",		/* clear screen and home cursor */
    "HOME":"[H",		/* home cursor */
    "BOLD":"[1m",		/* turn on bold (extra bright) attribute */
    "INVERSE":"[7m",		/* turn on reverse-video attribute */
    "BLINK":"[5m",		/* turn on blinking attribute */
    "RESET":"[m",		/* turn off all attributes */
    "TAB":"	",		/* move cursor to next hardware tab stop */
    "UNDERLINE":"[4m",	/* underline character overstrikes */
    "ESC":ESC,		/* Our escape character (so we can quote it) */
    ]),
  "atari":	/* I have one - Auronthas */
  ([
    "CLS":"E",
    "sr":"I",
    "up":"A",
    "ku":"A",
    "kd":"B",
    "kr":"C",
    "kl":"D",
    "kb":"",
    "BOLD":"p",
    "se":"q",
    "sc":"j",
    "rc":"k",
    "al":"L",
    "dl":"M",
    "vi":"f",
    "ve":"e",
    "is":"vq",
    "rs":"vEq",
    "BG":"c%c",
    "FG":"b%c",
    "ESC":ESC,		/* Our escape character (so we can quote it) */
    ]),
  "adm3a":
  ([
    "CLS":"",
    "ESC":ESC,		/* Our escape character (so we can quote it) */
    ]),    
  "colxterm":
  ([
    "BELL":"",		/* audible signal (bell) */
    "CLS":"[;H[2J",		/* clear screen and home cursor */
    "MOVE":"![%i%d;%dH",	/* screen-relative cursor motion to row m col n */
    "HOME":"[H",		/* home cursor */
    "INITTERM":"[1;24r[24;1H",/* terminal initialization string */
    "BOLD":"[1m",		/* turn on bold (extra bright) attribute */
    "INVERSE":"[7m",		/* turn on reverse-video attribute */
    "BLINK":"[5m",		/* turn on blinking attribute */
    "RESET":"[m",		/* turn off all attributes */
    "TAB":"	",		/* move cursor to next hardware tab stop */
    "UNDERLINE":"[4m",	/* underline character overstrikes */
    "BLACK":"[30m",
    "RED":"[31m",
    "GREEN":"[32m",
    "YELLOW":"[33m",
    "BLUE":"[34m",
    "MAGENTA":"[35m",
    "CYAN":"[36m",
    "GREY":"[37m",
    "WHITE":"[37m",

    "L_RED":"[1;31m",
    "L_GREEN":"[1;32m",
    "L_YELLOW":"[1;33m",
    "L_BLUE":"[1;34m",
    "L_MAGENTA":"[1;35m",
    "L_CYAN":"[1;36m",

    "B_BLACK":"[40m",
    "B_RED":"[41m",
    "B_GREEN":"[42m",
    "B_YELLOW":"[43m",
    "B_BLUE":"[44m",
    "B_MAGENTA":"[45m",
    "B_CYAN":"[46m",
    "B_GREY":"[47m",
    "B_WHITE":"[47m",
    "ESC":ESC,		/* Our escape character (so we can quote it) */
    ]),
  ]);

mapping
query_all_termcap_entries()
{
    return copy(termcap_entries);
}

mapping
query_termcap(string what) /* Added by Kniggit 930115 */
{
    if(termcap_entries && what)
	return copy(termcap_entries[what]);
}

mixed*
query_supported_termtypes() /* Added by Tim 960122 */
{
  if(termcap_entries)
    return keys(termcap_entries);
}

mixed
termcap_capability(string entry, string capability)
{
    mapping temp;

    if (temp = query_termcap(entry)) {
	if (!temp[capability] && temp["tc"])
	    return termcap_capability(temp["tc"], capability);
	return temp[capability];
    }
    return 0;
}

varargs string
termcap(string message, string capability, object ob)
{
    string t_type, t;

    if (!message)
	message = "";
    if (!ob && !(ob = this_player()))
	ob = previous_object();
    if ((t_type = (string)ob->get_env("TERM")) &&
	termcap_entries[t_type] &&
	(t = termcap_capability(t_type, capability)))
    {
	return (t ? t : "") + message + ((t = start_end[capability]) ?
	    termcap_capability(t_type, t) : "");
    }
    return message;
}	

varargs string
inverse (string str, object who)
{
    return str;
}

varargs string
blink (string str, object who) {
    return str;
}

varargs string
bold (string str, object who) {
    str=ESC "BOLD" ESC + str + ESC "RESET" ESC;
    return str;
}

varargs string
underscore (string str, object who) {
    return str;
}

varargs string
clear_screen (object who) {
    return ESC "CLS" ESC;
}

#if 0
string
termcap_format_line(string line, string ttype)
{
    string *words = explode(line, ESC);
    mapping tce = termcap_entries[ttype];
    int i = 1;

    if (mapp(tce) && stringp(ttype))
    {
	string w, end = tce["RESET"];
	int nl, ef, el = strlen(tce["RESET"])+1;
	while (i < sizeof(words)) {
	  words[i] = tce[words[i]];
	  i += 2;
	}
	w = implode(words, "");
	if (w[<el..<1] == end) {
	    w = w[0..<el+1];
	    ef = 1;
	}
	if (w[<1..<1] == "\n") {
	    nl = 1;
	    w = w[0..<2];
	}
	ef && w += end;
	nl && w += "\n";
	return w;
    }
    else
    {
	string *new_words = allocate((sizeof(words) / 2) + 1);
	int j = 0;
	i = 0;
	while (i < sizeof(words))
	{
	    new_words[j++] = words[i];
	    i += 2;
	}
	return implode(new_words - ({ 0 }), "");
    }
}
#endif
