/*  -*- LPC -*-  */
/*
 * If remove() in an object returns non-zero it will prevent it's destruction
 * unless 'force' is passed as the 2nd arg to destruct()
 */

varargs int
destruct(object ob, int force)
{
  if(!ob) return 0;

  if( !force &&  ob->remove() ) {
    return 0;
  } else {
    efun::destruct(ob);
    return 1;
  }
}
