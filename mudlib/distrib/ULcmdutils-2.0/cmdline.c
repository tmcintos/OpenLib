/*  -*- LPC -*-  */
/*
 * Copyright (C) 1996
 *	UltraLIB Administrators.  All rights reserved.
 *
 * 'UltraLIB Administrators' refers to the current administrators of the
 * UltraLIB Mudlib Development Project (herein UMDP).  Should the UMDP cease
 * to exist, ownership of this copyright should be reassigned by the last
 * administrators of the UMDP.  In the absence of such a reassignment,
 * ownership of this copyright will revert to Tim McIntosh.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the author nor the names of any co-contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE ULTRALIB ADMINISTRATORS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE ULTRALIB
 * ADMINISTRATORS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
// cmdline.c:  facilitates getting various things from the command line
// written by: Tim McIntosh (astasia@iastate.edu)
//
// Inspired by VikingMUD and other things...
//
// 05/03/96  Tim : optimized (?) some by changing the way flags are handled.

int _main(string arguments);
int flag(string flag);

// To be defined by object which inherits this
int main(string *argv, string *argv2);

private string *flags;               //  array of flags passed on command line
private string charflags;            //  single character flags

// Function:  _main()
// Arguments:  command line as taken from player
// Returns:  value returned by _main()
//
// This _main() function sits in front of the main() function to strip
// out flags and separate all the words on the command line

int
_main(string arguments)
{
  string* argv;          //  array of verb + all args passed to command
  string* argv2;         // array of all non-flag arguments

  flags = ({});
  charflags = "";

  if( arguments )
  {
    argv =  explode(arguments, " ");
    argv2 = filter(argv, (: strsrch($1, '-') :));  // remove if begins with '-'
  }
  else
  {
    argv =  ({ });
    argv2 = ({ });
  }

  foreach(string word in filter(argv, (: !strsrch($1, '-') :)))
  {
    if( word[1] != '-' )
      charflags += word;                   // character flags
    else
      flags += ({ word[2..] });            // word flags passed as --word
  }

  // Might as well catch this here

  if( flag("help") )
  {
    printf("Help for %s:\n\n", argv[0]);
    write(this_object()->help());
    write("\n");
    return 1;
  }

  return main(({ query_verb() }) + argv, argv2);
}

// Function:  flag()
// Arguments:  a flag ex: "l", "r", "help", etc.
// Returns:  1 if flag was passed
//           0 if flag was not passed

int
flag(string flag)
{
  if( sizeof(flag) == 1 )
    return strsrch(charflags, flag) != -1;
  else
    return member_array(flag, flags) != -1;
}
