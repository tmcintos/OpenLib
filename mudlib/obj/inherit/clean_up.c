/*  -*- LPC -*-  */
// clean_up.c:  

int
clean_up(int inh)
{
  if(inh)
    return 1;
  destruct(this_object());
  return 0;
}
