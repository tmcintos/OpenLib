// m_object_class.c:  Functions for setting/checking the type of an object
// Tim 23 Apr 96

#include "m_object_class.h"

//
// Global Variables
//
private static int object_class;         /* bitstring determines obj class */

nomask void
set_object_class(int obj_class)
{
  object_class = obj_class;
}

nomask int
query_object_class()
{
  return object_class;
}

nomask int
query_is_class(int obj_class)
{
  return (obj_class & object_class);
}
