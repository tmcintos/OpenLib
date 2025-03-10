/*
 * options.h: defines for the compile-time configuration of the MudOS driver
 */

#ifndef _OPTIONS_H_
#define _OPTIONS_H_

/****************************************************************************
 * EVERY time you change ANYTHING in this file, RECOMPILE from scratch.     *
 * (type "make clean" then "make" on a UNIX system) Failure to do so may    *
 * cause the driver to behave oddly.                                        *
 ****************************************************************************/

/* NOTES:
 * Many of the configurable options are now set via the configuration file
 *  that is specified as the first argument to the driver.
 * See port.h for those #defines related to portability (compatibility) if
 *  you have problems compiling on your system.
 * Removing an efun from func_spec.c usually removes most, if not all,
 *  of the code associated with it.
 */

/****************************************************************************
 *                              MALLOC                                      *
 *                             --------                                     *
 * for performance reasons, LP drivers have a variety of memory allocation  *
 * packages.  If you don't care, use the default one on your system:        *
 * #define SYSMALLOC, #undef the others.                                    *
 ****************************************************************************/

/* You must choose exactly one of these malloc packages:
 *     ~~~~
 * SYSMALLOC:
 *   * Built-in system malloc.
 *   * No statistics.
 *   * SYSMALLOC incurs no additional CPU or memory overhead.
 *
 * SMALLOC:
 *   * Satoria's smalloc.
 *   * Statistics available. (see wrappers and DO_MSTATS)
 *   * Faster than most system mallocs with modest ammount of memory overhead.
 *   * Can fall back onto system malloc if sbrk() not ok.
 *
 * BSDMALLOC:
 *   * BSD (Berkeley Software Distributions) malloc.
 *   * Statistics available. (see wrappers and DO_MSTATS)
 *   * Faster than SMALLOC but more memory overhead.
 *   * Requires sbrk().
 */
#define SYSMALLOC
#undef SMALLOC
#undef BSDMALLOC

/* You may optionally choose one (or none) of these malloc wrappers.  These
 * can be used in conjunction with any of the above malloc packages.
 *
 * WRAPPEDMALLOC:
 *   * Limited statistics.
 *   * Limited additional cpu overhead and no additional memory overhead.
 *
 * DEBUGMALLOC:
 *   * Statistics on precisely how much memory has been malloc'd (as well
 *     as the stats provided by WRAPPEDMALLOC).
 *   * Incurs a fair amount of overhead (both memory and CPU)
 */
#define WRAPPEDMALLOC
#undef DEBUGMALLOC

/* The following add certain bells and whistles to malloc: */

/*
 * SBRK_OK: do not define this unless SMALLOC is chosen above.
 *   Defining this causes smalloc to use the low level memory allocation
 *   routines, and to act as a malloc replacement.  Conversely, undef'ing
 *   SBRK_OK causes smalloc to act as a wrapper for the system malloc
 *   routines.
 *
 * Note:
 *   NeXTStep 3.x users should always #undef SBRK_OK.
 */
#undef SBRK_OK

/* DO_MSTATS: do not define this unless BSDMALLOC or SMALLOC is chosen above.
 *   Defining this causes those replacement mallocs to keep statistics that
 *   the malloc_status() efun will print out (including total memory
 *   allocated/used).
 */
#undef DO_MSTATS

/* DEBUGMALLOC_EXTENSIONS: defining this (in addition to DEBUGMALLOC) enables
 * the set_malloc_mask(int) and debugmalloc(string,int) efuns.  These two
 * efuns basically allow you to cause certain malloc's and free's (with tags
 * selected by a specified mask) to print debug information (addr, tag,
 * description, size) to stdio (in the shell that invoked the driver) or to a
 * file.  Not defining this does reduce the overhead of DEBUGMALLOC from 16
 * bytes per malloc down to 8.  This macro has no effect if DEBUGMALLOC isn't
 * defined.
 */
#undef DEBUGMALLOC_EXTENSIONS

/* CHECK_MEMORY: defining this (in addition to DEBUGMALLOC and
 * DEBUGMALLOC_EXTENSIONS) causes the driver to check for memory
 * corruption due to writing before the start or end of a block.  This
 * also adds the check_memory() efun.  Takes a considerable ammount
 * more memory.  Mainly for debugging.  
 */
#undef CHECK_MEMORY

/****************************************************************************
 *                          COMPATIBILITY                                   *
 *                         ---------------                                  *
 * The MudOS driver has evolved quite a bit over the years.  These defines  *
 * are mainly to preserve old behavior in case people didn't want to        *
 * rewrite the relevant portions of their code.                             *
 *                                                                          *
 * WARNING: If you are using software designed to run with the MudOS driver *
 *          it may assume certain settings of these options.  Check the     *
 *          instructions for details.                                       *
 ****************************************************************************/

/* HAS_STATUS_TYPE: old MudOS drivers had a 'status' type which was
 * identical to the 'int' type.  Define this to bring it back.
 */
#undef HAS_STATUS_TYPE

/* explode():
 *
 * The old behavior (#undef both of the below) strips any number of
 * delimiters at the start of the string, and one at the end.  So
 * explode("..x.y..z..", ".") gives ({ "x", "y", "", "z", "" })
 *
 * SANE_EXPLODE_STRING strips off at most one leading delimiter, and
 * still strips off one at the end, so the example above gives
 * ({ "", "x", "y", "", "z", "" }).
 *
 * REVERSIBLE_EXPLODE_STRING overrides SANE_EXPLODE_STRING, and makes
 * it so that implode(explode(x, y), y) is always x; i.e. no delimiters
 * are every stripped.  So the example above gives
 * ({ "", "", "x", "y", "", "z", "", "" }).
 */
#undef SANE_EXPLODE_STRING
#define REVERSIBLE_EXPLODE_STRING

/* CAST_CALL_OTHERS: define this if you want to require casting of call_other's;
 *   this was the default behavior of the driver prior to this addition.
 */
#undef CAST_CALL_OTHERS

/* NONINTERACTIVE_STDERR_WRITE: if defined, all writes/tells/etc to
 *   noninteractive objects will be written to stderr prefixed with a ']'
 *   (old behavior).
 */
#define NONINTERACTIVE_STDERR_WRITE

/* NO_LIGHT: define this to disable the set_light() and driver maintenance
 *   of light levels in objects.  You can simulate it via LPC if you want...
 */
#define NO_LIGHT

/* NO_ADD_ACTION: define this to remove add_action, commands, livings, etc.
   process_input() then becomes the only way to deal with player input. */
#undef NO_ADD_ACTION

/* NO_ENVIRONMENT: define this to remove the handling of object containment
   relationships by the driver */
#undef NO_ENVIRONMENT

/* NO_WIZARDS: for historical reasons, MudOS used to keep track of who
   is and isn't a wizard.  Defining this removes that completely.
   If this is defined, the wizardp() and related efuns don't exist */
#define NO_WIZARDS

/* OLD_TYPE_BEHAVIOR: reintroduces a bug in type-checking that effectively
 * renders compile time type checking useless.  For backwards compatibility.
 */
#undef OLD_TYPE_BEHAVIOR

/* OLD_RANGE_BEHAVIOR: define this if you want negative indexes in string
 * or buffer range values (not lvalue, i.e. x[-2..-1]; for e.g. not 
 * x[-2..-1] = foo, the latter is always illegal) to mean counting from the 
 * end 
 */
#undef OLD_RANGE_BEHAVIOR

/* OLD_ED: ed() efun backwards compatible with the old version.  The new
 * version requires/allows a mudlib front end.
 */
#undef OLD_ED

/****************************************************************************
 *                           MISCELLANEOUS                                  *
 *                          ---------------                                 *
 * Various options that affect the way the driver behaves.                  *
 *                                                                          *
 * WARNING: If you are using software designed to run with the MudOS driver *
 *          it may assume certain settings of these options.  Check the     *
 *          instructions for details.                                       *
 ****************************************************************************/

/*
 * Some minor tweaks that make it a bit easier to run code designed to run
 * on LPmud 3.2/3.2.1.  Currently has the following effects:
 * 
 * . m_indices() and m_values() are synonyms for keys() and values(),
 *   respectively
 * . map_delete() returns it's first argument
 * . inherit_list() means deep_inherit_list(), not shallow_inherit_list()
 * . heart_beat_info() is a synonym for heart_beats()
 */
#undef COMPAT_32

/*
 * Keep statistics about allocated strings, etc.  Which can be veiwed with
 * the mud_status() efun.  If this is off, mud_status() and memory_info()
 * ignore allocated strings, but string operations run faster.
 */
#undef STRING_STATS

/* LOG_CATCHES: define this to cause errors that are catch()'d to be
 *   sent to the debug log anyway.
 *
 * On by default, because newer libs use catch() alot, and it's confusing
 * if the errors don't show up in the logs.
 */
#define LOG_CATCHES

/* ARGUMENTS_IN_TRACEBACK: prints out function call arguments in error
 *   tracebacks, to aid in debugging.  Note: it prints the values of
 *   the arguments at the time of the error, not when the function
 *   was called.  It looks like this:
 *
 * Failed to load file: read_buffer
 * program: command/update.c, object: command/update line 15
 * '    commandHook' in '        clone/user.c' ('        clone/user#1')line 72
 * arguments were ("/read_buffer.c")
 * '           main' in '    command/update.c' ('      command/update')line 15
 * arguments were ("/read_buffer.c")
 *
 * The only down side is some people like their logs shorter
 */
#define ARGUMENTS_IN_TRACEBACK

/* LOCALS_IN_TRACEBACK: similar to ARGUMENTS_IN_TRACEBACK, but for local
 *   variables.  The output looks more or less like:
 *
 * locals: 1, "local_value"
 *
 * Same as above.  Tends to produce even longer logs, but very useful for
 * tracking errors.
 */
#define LOCALS_IN_TRACEBACK

/* MUDLIB_ERROR_HANDLER: If you define this, the driver doesn't do any
 *   handling of runtime errors, other than to turn the heartbeats of
 *   objects off.  Information about the error is passed in a mapping
 *   to the error_handler() function in the master object.  Whatever is
 *   returned is put in the debug.log.
 *
 * A good mudlib error handler is one of the best tools for tracking down
 * errors.  Unfortunately, you need to have one.  Check the testsuite or
 * other libs for an example.
 */
#define MUDLIB_ERROR_HANDLER

/* OPTIMIZE_FUNCTION_TABLE_SEARCH: define this if you want the function
 *   table to be sorted for faster lookups (ie binary search).  The flipside
 *   of this is that there is some overhead in maintaining the sorted table.
 */
#define OPTIMIZE_FUNCTION_TABLE_SEARCH

/* CONFIG_FILE_DIR specifies a directory in which the driver will search for
 *   config files by default.  If you don't wish to use this define, you may
 *   always specify a full path to the config file when starting the driver.
 */
#define CONFIG_FILE_DIR "/home/tim/mud/etc"

/* DEFAULT_PRAGMAS:  This should be a sum of pragmas you want to always
 * be on, i.e.
 *
 * #define DEFAULT_PRAGMAS PRAGMA_STRICT_TYPES + PRAGMA_SAVE_TYPES
 *
 * will make every LPC file behave as if it had the lines:
 * #pragma strict_types
 * #pragma save_types
 *
 * for no default pragmas:
 * #define DEFAULT_PRAGMAS 0
 *
 * If you don't know what these are, 0 is a good choice.
 *
 * Supported pragmas:
 * PRAGMA_STRICT_TYPES: enforces strict type checking
 * PRAGMA_WARNINGS:     issues warnings about various dangerous things in
 *                      your code
 * PRAGMA_SAVE_TYPES:   save the types of function arguments for checking
 *                      calls to functions in this object by objects that
 *                      inherit it.
 * PRAGMA_SAVE_BINARY:  save a compiled binary version of this file for
 *                      faster loading next time it is needed.
 * PRAGMA_OPTIMIZE:     make a second pass over the generated code to
 *                      optimize it further.  currently does jump threading.
 * PRAGMA_ERROR_CONTEXT:include some text telling where on the line a
 *                      compilation error occured.
 */
#define DEFAULT_PRAGMAS PRAGMA_WARNINGS + PRAGMA_OPTIMIZE + PRAGMA_SAVE_BINARY

/* NO_RESETS: completely disable the periodic calling of reset() */
#undef NO_RESETS

/* LAZY_RESETS: if this is defined, an object will only have reset()
 *   called in it when it is touched via call_other() or move_object()
 *   (assuming enough time has passed since the last reset).  If LAZY_RESETS
 *   is #undef'd, then reset() will be called as always (which guaranteed that
 *   reset would always be called at least once).  The advantage of lazy
 *   resets is that reset doesn't get called in an object that is touched
 *   once and never again (which can save memory since some objects won't get
 *   reloaded that otherwise would).
 */
#define LAZY_RESETS

/* SAVE_EXTENSION: defines the file extension used by save_object().
 *   and restore_object().  Some sysadmins run scripts that periodically
 *   scan for and remove files ending in .o (but many mudlibs are already
 *   set up to use .o thus we leave .o as the default).
 */
#define SAVE_EXTENSION ".o"

/* NO_ANSI: define if you wish to disallow users from typing in commands that
 *   contain ANSI escape sequences.  Defining NO_ANSI causes all escapes
 *   (ASCII 27) to be replaced with a space ' ' before the string is passed
 *   to the action routines added with add_action.
 *
 * If you anticipate problems with users intentionally typing in ANSI codes
 * to make your terminal flash, etc define this.
 */
#undef NO_ANSI

/* OPCPROF: define this if you wish to enable OPC profiling. Allows a dump
 *   of the # of times each efun is invoked (via the opcprof() efun).
 */
#undef OPCPROF

/* OPCPROF_2D: define this if you wish to enable 2-D OPC profiling. Allows a 
 *   dump of the # of times each *pair* of eoperators is invoked.
 *
 * You can't use this and OPCPROF at the same time.
 */
#undef OPCPROF_2D

/* TRAP_CRASHES:  define this if you want MudOS to call crash() in master.c
 *   and then shutdown when signals are received that would normally crash the
 *   driver.
 */
#define TRAP_CRASHES

/* DROP_CORE: define this if you want the driver to attempt to create
 *   a core file when it crashes via the crash_MudOS() function.  This
 *   define only has an affect if -DDEBUG isn't defined in the makefile
 *   (except for the SIGINT and SIGTERM signals which are always trapped).
 *
 * [NOTE: keep this undefined for now since it seems to hang some machines
 *  upon crashing (some DECstations apparently).  If you want to get a core
 *  file, undef'ing TRAP_CRASHES should work.]
 */
#undef DROP_CORE

/* THIS_PLAYER_IN_CALL_OUT: define this if you wish this_player() to be
 *   usable from within call_out() callbacks.
 */
#undef THIS_PLAYER_IN_CALL_OUT

/* CALLOUT_HANDLES: If this is defined, call_out() returns an integer, which
 * can be passed to remove_call_out() or find_call_out().  Removing call_outs
 * by name is still allowed, but is significantly less efficient, and also
 * doesn't work for function pointers.  This option adds 4 bytes overhead
 * per callout to keep track of the handle.
 */
#define CALLOUT_HANDLES

/* FLUSH_OUTPUT_IMMEDIATELY: Causes output to be written to sockets
 * immediately after being generated.  Useful for debugging.  
 */
#undef FLUSH_OUTPUT_IMMEDIATELY

/* PRIVS: define this if you want object privledges.  Your mudlib must
 *   explicitly make use of this functionality to be useful.  Defining this
 *   this will increase the size of the object structure by 4 bytes (8 bytes
 *   on the DEC Alpha) and will add a new master apply during object creation
 *   to "privs_file".  In general, priveleges can be used to increase the
 *   granularity of security beyond the current root uid mechanism.
 *
 * [NOTE: for those who'd rather do such things at the mudlib level, look at
 *  the inherits() efun and the 'valid_object' apply to master.]
 */
#define PRIVS

/* INTERACTIVE_CATCH_TELL: define this if you want catch_tell called on
 *   interactives as well as NPCs.  If this is defined, user.c will need a
 *   catch_tell(msg) method that calls receive(msg);
*/
#undef INTERACTIVE_CATCH_TELL

/* RESTRICTED_ED: define this if you want restricted ed mode enabled.
 */
#define RESTRICTED_ED

/* NO_SHADOWS: define this if you want to disable shadows in your driver.
 */
#undef NO_SHADOWS

/* SNOOP_SHADOWED: define this if you want snoop to report what is
 *   sent to the player even in the event that the player's catch_tell() is
 *   shadowed and the player may not be seeing what is being sent.  Messages
 *   of this sort will be prefixed with $$.
 */
#undef SNOOP_SHADOWED

/* RECEIVE_SNOOP: define this if you want snoop text to be sent to
 *   the receive_snoop() function in the snooper object (instead of being
 *   sent directly via add_message()).  This is useful if you want to
 *   build a smart client that does something different with snoop messages.
 */
#define RECEIVE_SNOOP

/* PROFILE_FUNCTIONS: define this to be able to measure the CPU time used by
 *   all of the user-defined functions in each LPC object.  Note: defining
 *   this adds three long ints (12 bytes on 32-bit machines) to the function
 *   header structs.  Also note that the resolution of the getrusage() timer
 *   may not be high enough on some machines to give non-zero execution
 *   times to very small (fast) functions.  In particular if the clock
 *   resolution is 1/60 of a second, then any time less than approxmately 15k
 *   microseconds will resolve to zero (0).
 */
#undef PROFILE_FUNCTIONS

/* NO_BUFFER_TYPE: if this is #define'd then LPC code using the 'buffer'
 *   type won't be allowed to compile (since the 'buffer' type won't be
 *   recognized by the lexer.
 */
#undef NO_BUFFER_TYPE

/* BINARIES: define this to enable the 'save_binary' pragma.
 *   This pragma, when set in a program, will cause it to save a
 *   binary image when loaded, so that subsequent loadings will
 *   be much faster.  The binaries are saved in the directory
 *   specified in the configuration file.  The binaries will not
 *   load if the LPC source or any of the inherited or included
 *   files are out of date, in which case the file is compiled
 *   normally (and may save a new binary).
 *
 *   In order to save the binary, valid_save_binary() is called
 *   in master.c, and is passed the name of the source file.  If
 *   this returns a non-zero value, the binary is allowed to be
 *   saved.  Allowing any file by any wizard to be saved as a
 *   binary is convenient, but may take up a lot of disk space.
 */
#define BINARIES

/* ARRAY_RESERVED_WORD: If this is defined then the word 'array' can
 *   be used to define arrays, as in:
 *
 * int array x = ({ .... });
 *
 * A side effect is that array cannot be a variable or function name.
 */
#undef ARRAY_RESERVED_WORD

/****************************************************************************
 *                              PACKAGES                                    *
 *                              --------                                    *
 * Defining some/all of the following add certain efuns, and sometimes      *
 * add/remove code from the driver.                                         *
 *                                                                          *
 * if PACKAGE_XYZZY is defined here, then the code in packages/xyzzy.c      *
 * and the efuns in packages/xyzzy_spec.c will be added to the driver.      *
 ****************************************************************************/

/* various miscellaneous efuns */
#define PACKAGE_CONTRIB

/* efuns that are only of use to those that know something about driver
   internals */
#undef PACKAGE_DEVELOP

/* PACKAGE_MATH: determines whether or not the math efuns (for floats) are
   included.
 */
#define PACKAGE_MATH

/* PACKAGE_MATRIX: determines whether or not the 3d graphics efuns (for floats)
 *   are included - see packages/matrix.spec for a list.
 */
#undef PACKAGE_MATRIX

/* PACKAGE_MUDLIB_STATS: define this to enable domain and author stats
 *   maintenance by the driver.  These mudlib stats are more domain
 *   based than user based, and replaces the traditional wiz_list stats.
 */
#undef PACKAGE_MUDLIB_STATS

/* PACKAGE_SOCKETS: define this to enable the socket efunctions.  This
 *   causes HAS_SOCKETS to be defined for all LPC objects.
 */
#define PACKAGE_SOCKETS

/* PACKAGE_PARSER: Natural language parsing efuns for interactive fiction
 *   type applications
 */
#define PACKAGE_PARSER

/* PACKAGE_EXTERNAL: Allows the driver to exec() commands specified in the
 * config file.
 */
#define PACKAGE_EXTERNAL

/* PACKAGE_DB: efuns for external database access */
#undef PACKAGE_DB

/* If PACKAGE_DB is defined above, you must pick ONE of the following supported
 * databases
 */
#ifdef PACKAGE_DB
#define MSQL		/* MiniSQL, it's small; it's free */
#endif

/****************************************************************************
 *                            UID PACKAGE                                   *
 *                            -----------                                   *
 * UIDS are the basis for some mudlib security systems.  Basically, they're *
 * preserved for backwards compatibility, as several ways of breaking       *
 * almost any system which relies on them are known.  (No, it's not a flaw  *
 * of uids; only that b/c of the ease with which LPC objects can call       *
 * each other, it's far to easy to leave holes)                             *
 *                                                                          *
 * If you don't care about security, the first option is probably what you  *
 * want.                                                                    *
 ****************************************************************************/

/*
 * PACKAGE_UIDS: define this if you want a driver that does use uids.
 *
 */
#undef PACKAGE_UIDS

/* AUTO_SETEUID: when an object is created it's euid is automatically set to
 *   the equivalent of seteuid(getuid(this_object())).  undef AUTO_SETEUID
 *   if you would rather have the euid of the created object be set to 0.
 */
#undef AUTO_SETEUID

/* AUTO_TRUST_BACKBONE: define this if you want objects with the backbone
 *   uid to automatically be trusted and to have their euid set to the uid of
 *   the object that forced the object's creation.
 */
#undef AUTO_TRUST_BACKBONE

/*************************************************************************
 *                       FOR EXPERIENCED USERS                           *
 *                      -----------------------                          *
 * Most of these options will probably be of no interest to many users.  *
 *************************************************************************/

/* HEARTBEAT_INTERVAL: define heartbeat interval in microseconds (us).
 *   1,000,000 us = 1 second.  The value of this macro specifies
 *   the frequency with which the heart_beat method will be called in
 *   those LPC objects which have called set_heart_beat(1).
 *
 * [NOTE: if ualarm() isn't available, alarm() is used instead.  Since
 *  alarm() requires its argument in units of a second, we map 1 - 1,000,000 us
 *  to an actual interval of one (1) second and 1,000,001 - 2,000,000 maps to
 *  an actual interval of two (2) seconds, etc.]
 */
#define HEARTBEAT_INTERVAL 2000000

/* 
 * CALLOUT_CYCLE_SIZE: This is the number of slots in the call_out list.
 * It should be approximately the average number of active call_outs, or
 * a few times smaller.  It should also be a power of 2, and also be relatively
 * prime to any common call_out lengths.  If all this is too confusing, 32
 * isn't a bad number :-)
 */
#define CALLOUT_CYCLE_SIZE 32

/* LARGEST_PRINTABLE_STRING: defines the size of the vsprintf() buffer in
 *   comm.c's add_message(). Instead of blindly making this value larger,
 *   mudlib should be coded to not send huge strings to users.
 */
#define LARGEST_PRINTABLE_STRING 8192

/* MESSAGE_BUFFER_SIZE: determines the size of the buffer for output that
 *   is sent to users.
 */
#define MESSAGE_BUFFER_SIZE 4096

/* APPLY_CACHE_BITS: defines the number of bits to use in the call_other cache
 *   (in interpret.c).  Somewhere between six (6) and ten (10) is probably
 *   sufficient for small muds.
 */
#define APPLY_CACHE_BITS 10

/* CACHE_STATS: define this if you want call_other (apply_low) cache 
 * statistics.  Causes HAS_CACHE_STATS to be defined in all LPC objects.
 */
#undef CACHE_STATS

/* TRACE: define this to enable the trace() and traceprefix() efuns.
 *   (keeping this undefined will cause the driver to run faster).
 */
#undef TRACE

/* LPC_TO_C: define this to enable LPC->C compilation.
 *
 * [NOTE: BINARIES must also be defined for LPC->C to work.  Actually
 *  using binaries is not required, though.]
 */
#undef LPC_TO_C

/* RUNTIME_LOADING: On systems which support it, it allows LPC->C compilation
 * 'on the fly' without having to recompile the driver.
 *
 * Note: This currently only works on machines that have the dlopen() system
 * call.  SunOS and IRIX do, as do a number of others.  AIX and Ultrix don't.
 * Linux does if you are using ELF.
 */
#undef RUNTIME_LOADING

/* TRACE_CODE: define this to enable code tracing (the driver will print
 *   out the previous lines of code to an error) eval_instruction() runs about
 *   twice as fast when this is not defined (for the most common eoperators).
 */
#undef TRACE_CODE

/* HEART_BEAT_CHUNK: The number of heart_beat chunks allocated at a time.
 * A large number wastes memory as some will be sitting around unused, while
 * a small one wastes more CPU reallocating when it needs to grow.  Default
 * to a middlish value.
 */
#define HEART_BEAT_CHUNK      32

/* Some maximum string sizes
 */
#define SMALL_STRING_SIZE     100
#define LARGE_STRING_SIZE     1000
#define COMMAND_BUF_SIZE      2000

/* Number of levels of nested datastructures allowed -- this limit prevents
 * crashes from occuring when saving objects containing variables containing
 * recursive datastructures (with circular references).
 */
#define MAX_SAVE_SVALUE_DEPTH 25

/* Miscellaneous config options that should probably be in the runtime
 * config file.
 */
/* MAX_LOCAL: maximum number of local variables allowed per LPC function */
#define CFG_MAX_LOCAL_VARIABLES		25

#define CFG_EVALUATOR_STACK_SIZE 	1000
#define CFG_COMPILER_STACK_SIZE		200
#define CFG_MAX_CALL_DEPTH		50
/* This must be one of 4, 16, 64, 256, 1024, 4096 */
#define CFG_LIVING_HASH_SIZE		256

/* NEXT_MALLOC_DEBUG: define this if using a NeXT and you want to enable
 *   the malloc_check() and/or malloc_debug() efuns.  Run the 'man malloc_debug'
 *   command on the NeXT to find out what the arguments to malloc_debug(int)
 *   mean.  The malloc_check() efun calls the NeXT NXMallocCheck() system
 *   call which does a consistency check on malloc's data structures (this
 *   consistency check is done at each malloc() and free() for certain
 *   malloc_debug() levels).  A non-zero return value indicates there was
 *   a consistency problem.  For those NeXT users wanting a bit more
 *   performance out of malloc, try defining NEXT_MALLOC_DEBUG and calling the
 *   malloc_debug(-1) efun (with an arg of -1).  This will turn all
 *   malloc debugging off and call malloc_singlethreaded() which the NeXT
 *   malloc man page claims can make NeXT system malloc 10% to 15% faster.
 *
 * [NOTE: This #define has no affect on the driver if not using the
 *  NeXTSTEP OS.]
 *
 * Warning: if you use a NeXT and define NEXT_MALLOC_DEBUG, be sure to
 *          protect the use of the malloc_check() and malloc_debug() efuns
 *          since setting certain debug levels can cause malloc() and free()
 *          to become _very_ slow (protect efuns by using simul_efuns and
 *          valid_override).
 *
 * [NOTE: malloc_debug(6) is a good compromise between efficiency and
 *  completeness of malloc debugging (malloc/free will be about half as fast).]
 */
#undef NEXT_MALLOC_DEBUG

#endif
