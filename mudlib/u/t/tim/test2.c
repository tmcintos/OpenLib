create()
{
  foreach((int x) in ({ 1, 2, 3, 4 }))
    tell_object(find_player("tim"), ""+x);
}
