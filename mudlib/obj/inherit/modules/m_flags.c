/*  -*- LPC -*-  */
// m_flags.c:  Functions for setting/testing flags in objects
// Tim 23 Apr 96

//
// Global Variables
//
private mapping flags;             /* Mapping of flag names to integers */

//
// Prototypes
//
void initialize();            // make sure to call this in your routine
int query_flag(string);
void set_flag(string, int);
void clear_flag(string);

void
initialize()
{
  flags = ([ ]);
}

nomask int
query_flag(string flag)
{
  return flags[flag];
}

nomask void
set_flag(string flag, int lifetime)
{
  flags[flag] = lifetime;
}

nomask void
clear_flag(string flag)
{
  map_delete(flags, flag);
}
