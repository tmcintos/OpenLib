/*  -*- LPC -*-  */
#ifndef _CMDLINE_H
#define _CMDLINE_H
// cmdline.h:  A replacement for command.h which provides facilities for
//             stripping flags from the command line. (see cmd_module.c)
//
// written by: Tim McIntosh (astasia@iastate.edu)
//
// This file is part of the UltraLib distribution mudlib.
// It may be used and modified as long as this header remains intact.
// The UltraLib mudlib is available via FTP at knapp38.res.iastate.edu
//
// #include this and write _main() as declared below for your main function.
//
// int _main(string *argv, string *argv2);
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

#include <command.h>
#include <dirs.h>

inherit INHERIT_DIR "/cmd_module";

#endif
