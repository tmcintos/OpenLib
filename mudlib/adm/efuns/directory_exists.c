int directory_exists(string dirname)
{
  return (file_size(dirname) == -2);
}
