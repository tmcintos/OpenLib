/*  -*- LPC -*-  */
// clean_up.c:  destruct this object if it isn't inherited.  If it is, come
//              back later and check again.

int
clean_up(int inh)
{
  if(inh)
    return 1;
  destruct(this_object());
  return 0;
}
