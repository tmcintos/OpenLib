int file_exists(string filename)
{
  seteuid( geteuid(previous_object()) );

  return (file_size(filename) >= 0);
}
