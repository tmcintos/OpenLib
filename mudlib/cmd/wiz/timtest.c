/*  -*- LPC -*-  */
#include <command.h>

// basically a watered down version of get_words that makes sure everything
// will be ok
int
check_syntax(string line)
{
  int double_quotes, left_quote, right_quote;
  int escaped;

  for(int i = 0; i < sizeof(line); i++) {
    if(escaped) {
      escaped = 0;
      continue;
    } else if( (double_quotes && line[i] != '"') ||
	       (left_quote && line[i] != '`')     ||
	       (right_quote && line[i] != '\'') )
      continue;
    // if we're in the middle of a quoted string, ignore anything except a
    // closing quote

    switch(line[i]) {
    case '\\':
      escaped = 1;
      break;
    case '"':
      double_quotes = !double_quotes;
      break;
    case '`':
      left_quote = !left_quote;
      break;
    case '\'':
      right_quote = !right_quote;
      break;
    }
  }
  // if there are unmatched quotes, fail
  if(double_quotes || left_quote || right_quote) return 0;

  return 1;
}
string*
get_words(string line)
{
  int double_quotes, left_quote, right_quote, escaped;
  string* words = ({});
  int idx;

  for(int i = 0; i < sizeof(line); i++) {
    if(escaped) {
      escaped = 0;
      continue;
    } else if( (double_quotes && line[i] != '"') ||
	       (left_quote && line[i] != '`')     ||
	       (right_quote && line[i] != '\'') )
      continue;
    // if we're in the middle of a quoted string, ignore anything except a
    // closing quote

    switch(line[i]) {
    case '\\': /* Backslash */
      escaped = 1;
      break;
    case '"': /* Double Quote */
      if(!double_quotes) {
	if(idx != i) {
	  words += ({ replace_string(line[idx..i-1], " ", "") });
	  idx = i+1;
	}
	idx = i;
      } else {
	words += ({ line[idx..i] });
        idx = i+1;
      }
      double_quotes = !double_quotes;
      break;
    case '`': /* Left Quote */
      if(!left_quote) {
	if(idx != i) {
	  words += ({ replace_string(line[idx..i-1], " ", "") });
	  idx = i+1;
	}
	idx = i;
      } else {
	words += ({ line[idx..i] });
        idx = i+1;
      }
      left_quote = !left_quote;
      break;
    case '\'': /* Right Quote */
      if(!right_quote) {
	if(idx != i) {
	  words += ({ replace_string(line[idx..i-1], " ", "") });
	  idx = i+1;
	}
	idx = i;
      } else {
	words += ({ line[idx..i] });
        idx = i+1;
      }
      right_quote = !right_quote;
      break;
    case ' ': /* SPACE */
      if(idx != i) {
	words += ({ replace_string(line[idx..i-1], " ", "") });
	idx = i+1;
      }
    }
  }
  if(idx < sizeof(line))
    words += ({ replace_string(line[idx..], " ", "") });

  return words - ({ "" });
}

string
remove_slashes(string str)
{
  int found;

  for(int i = 0; i < sizeof(str); i++) {
    if( str[i] == '\\' )
      str[i..i] = "";
  }

  return str;
}

int
main(string str)
{
//  write(remove_slashes(str));
  printf("arg: %O\n", str);
  printf("Check syntax: %i\n", check_syntax(str));
  printf("Words: %O\n", get_words(str));
  return 1;
}
