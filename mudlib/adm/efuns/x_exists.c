/* 
 * 3 existence testing functions written by Tim.
 */

#include <login.h>

/*
 * file_exists(file):  returns 1 if 'file' is an existing file
 */

int
file_exists(string filename)
{
  return (file_size(filename) >= 0);
}

/*
 * directory_exists(dir):  returns 1 if directory 'dir' exists; 0 otherwise.
 */

int
directory_exists(string dirname)
{
  return (file_size(dirname) == -2);
}

/*
 * user_exists(x):  returns 1 if user with username 'x' exists; 0 otherwise.
 *                  username should be lower case.
 */

int
user_exists(string x)
{
  return file_exists(CONNECTION_DIR "/" + x[0..0] + "/" + x + ".o");
}
