/*  -*- LPC -*-  */
// m_clean_up.c:  Function for object self-removal
// Tim 23 Apr 96

// if item is inherited, has environment, or contains living objects, then
// don't destruct it.

int
clean_up(int inh)
{
  if( inh ) return 1;
  if( environment() ) return 1;
  if( sizeof(filter_array(all_inventory(this_object()), (: living :))) )
    return 1;

  destruct();
}
