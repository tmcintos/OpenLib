int file_exists(string filename)
{
  return (file_size(filename) >= 0);
}
