/*  -*- LPC -*-  */

/*
 * Copyright (C) 1996
 *	UltraLIB Administrators.  All rights reserved.
 *
 * 'UltraLIB Administrators' refers to the current administrators of the
 * UltraLIB Mudlib Development Project (herein UMDP).  Should the UMDP cease
 * to exist, ownership of this copyright should be reassigned by the last
 * administrators of the UMDP.  In the absence of such a reassignment,
 * ownership of this copyright will revert to Tim McIntosh.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the author nor the names of any co-contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE ULTRALIB ADMINISTRATORS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE ULTRALIB
 * ADMINISTRATORS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

// to_english.c:  functions for changing an int to a readable string of words.
//
// Created by Tim@Dysfunctional Mud
//
//    09/16/95  added recursive code to handle numbers up to 999,999,999
//              also fixed to handle negative numbers
// string to_english(int) returns the English representation of any integer

private:

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

public:

string
to_english(int integer)
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
    ret += to_english(tmp) + " billion" + (integer ? ", " : "");
  }
    
  if((tmp = integer / 1000000)) {                  // 1000000 or larger
    integer %= 1000000;
    ret += to_english(tmp) + " million" + (integer ? ", " : "");
  }

  if((tmp = integer / 1000)) {                     // 1000 or larger
    integer %= 1000;
    ret += to_english(tmp) + " thousand" + (integer ? ", " : ",");
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
