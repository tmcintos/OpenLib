object*
livings()
{
  return objects((: $1->is_living() :));
}

//
// This should be hacked out eventually since we can just do this
// directly
int
living(object ob)
{
  return ob->is_living();
}
