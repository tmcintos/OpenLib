/*  -*- LPC -*-  */
// m_dimensions.c:  Functions for objects with dimensions of weight and bulk
// Tim 23 Apr 96

#include "m_dimensions.h"

//
// Global Variables
//
private static int weight;               /* weight of this object */
private static int bulk;                 /* bulk of this object */

nomask void
set_weight(int pounds)
{
  weight = pounds;
}

int
query_base_weight()
{
  return weight;
}

int
query_weight()
{
  return query_base_weight();
}

nomask void
set_bulk(int units)
{
  bulk = units;
}

int
query_base_bulk()
{
  return bulk;
}

int
query_bulk()
{
  return query_base_bulk();
}
