/*  -*- LPC -*-  */
// inttostr.c:  Tim McIntosh
//   functions for changing an int to a readable string of words
//    09/16/95  added recursive code to handle numbers up to 999,999,999
//              also fixed to handle negative numbers

// This is what you normally want to call
// to_string() returns the word representation of a number from
// converts any integer to its string representation

string to_string(int);

// Private...
string digit_to_word(int i);       // Given int 0-10 returns word
string digit_to_prefix(int i);     // Given int 10,20,30..90 returns word
string teen_to_word(int i);        // Given int 11-19 returns word

string
to_string(int integer)
{
  int tmp;
  string ret = "";

  if(!integer) return "zero";                      // special case

  if(integer < 0) {                                // handle negative numbers
    integer *= -1;                                 // can only happen once
    ret = "negative ";
  }

  if((tmp = integer / 1000000000)) {
    integer %= 1000000000;
    ret += to_string(tmp) + " billion" + (integer ? ", " : "");
  }
    
  if((tmp = integer / 1000000)) {                  // 1000000 or larger
    integer %= 1000000;
    ret += to_string(tmp) + " million" + (integer ? ", " : "");
  }

  if((tmp = integer / 1000)) {                     // 1000 or larger
    integer %= 1000;
    ret += to_string(tmp) + " thousand" + (integer ? ", " : ",");
  }

  if((tmp = integer / 100)) {                       // if 100 or larger
    integer %= 100;
    ret += digit_to_word(tmp) + " hundred" + (integer ? " " : "");
  }

  if(integer < 10)                                   // if 1-9
    return ret + digit_to_word(integer);

  if(integer < 20)                                   // if it's a teen
    return ret + teen_to_word(integer);              // 10-19

  if(!(tmp = integer % 10))                       // 20 or more
    return ret + digit_to_prefix(integer / 10);

  return ret + digit_to_prefix(integer / 10) +"-"+ digit_to_word(tmp);
}

string
digit_to_word(int i)
{
  switch(i) {
  case 0 :
    return "";
  case 1 :
    return "one";
  case 2 :
    return "two";
  case 3 :
    return "three";
  case 4 :
    return "four";
  case 5 :
    return "five";
  case 6 :
    return "six";
  case 7 :
    return "seven";
  case 8 :
    return "eight";
  case 9 :
    return "nine";
  default :
    return "many";
  }
}

string
digit_to_prefix(int i)
{
  switch(i) {
  case 2:
    return "twenty";
  case 3:
    return "thirty";
  case 4:
    return "forty";
  case 5:
    return "fifty";
  case 6:
    return "sixty";
  case 7:
    return "seventy";
  case 8:
    return "eighty";
  case 9:
    return "ninety";
  default:
    return "many";
  }
}

string
teen_to_word(int i)
{
  switch(i) {
  case 10:
    return "ten";
  case 11 :
    return "eleven";
  case 12 :
    return "twelve";
  case 13 :
    return "thirteen";
  case 14 :
    return "fourteen";
  case 15 :
    return "fifteen";
  case 16 :
    return "sixteen";
  case 17 :
    return "seventeen";
  case 18 :
    return "eighteen";
  case 19 :
    return "nineteen";
  default :
    return "umpteen";
  }
}
