/*  -*- LPC -*-  */
// m_value.c:  Functions for valuable objects
// Tim 23 Apr 96

#include <money.h>

//
// Global Variables
//
private static int* value;               /* array of size COIN_TYPES */

//
// Prototypes
//
void initialize();     // make sure to call this in your routine
int* query_value();
int set_value(int*);

void
initialize()
{
  value = allocate(COIN_TYPES);
  for(int i = 0; i < COIN_TYPES; i++)
    value[i] = 0;
}

nomask int*
query_value()
{
  return copy(value);
}

nomask int
set_value(int* money)
{
  if( sizeof(money) != COIN_TYPES ) return 0;

  for(int i = 0; i < COIN_TYPES; i++)
    value[i] = money[i];

  return 1;
}
