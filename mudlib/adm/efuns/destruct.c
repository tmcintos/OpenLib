/*  -*- LPC -*-  */
int
destruct(object ob)
{
  if(!ob)
    return 0;

  if(ob->remove() == -1)
    return 0;

  efun::destruct(ob);
  return 1;
}
