/*  -*- LPC -*-  */
// m_light.c:  Functions for objects which contain or produce light
//             Light level for rooms/containers, light given off for objects
// Tim 23 Apr 96

//
// Global Variables
//
private static int light;                /* presence of light */

//
// Prototypes
//
void set_light(int);
int query_light();

nomask void
set_light(int level)
{
  light = level;
}

nomask int
query_light()
{
  return light;
}
