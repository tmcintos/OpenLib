int directory_exists(string dirname)
{
  seteuid( geteuid(previous_object()) );

  return (file_size(dirname) == -2);
}
