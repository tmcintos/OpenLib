varargs
string
pronoun(object ob, int possessive)
{
  int sex = ob->query_sex();

  if(possessive) {
    if(!sex)
      return "its";
    else if(sex == -1)
      return "her";
    else
      return "his";
  } else {
    if(!sex)
      return "it";
    else if(sex == -1)
      return "she";
    else
      return "he";
  }
}
