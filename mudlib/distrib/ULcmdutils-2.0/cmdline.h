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
#ifndef _COMMAND_H
#define _COMMAND_H
// cmdline.h:  A replacement for command.h which provides facilities for
//             stripping flags from the command line. (see cmdline.c)
//
// written by: Tim McIntosh (astasia@iastate.edu)
//
// #include this and write main() as declared below for your main function.
//
// int main(string *argv, string *argv2);
//
// arguments passed:
//          argv:  an array of all the words passed to the command with nothing
//                 removed.  argv[0] is always the verb.
//          argv2: an array of all the words passed to the command with flags
//                 removed--the verb is not included in this array
//
//          flag(string str) tells you if the flag 'str' is present.
//                           Flags that are more than 1 char. must be passed
//                           with 2 leading dashes. e.g. --help
//
// NOTE: --help is a special flag which will cause help info to be printed
// as returned by help() in the command object.

#include <mudlib.h>
#include <dirs.h>
#include <modules.h>

inherit DAEMON;

inherit M_CLEAN_UP;
inherit INHERIT_DIR "/cmdline";

#endif /* _COMMAND_H */
