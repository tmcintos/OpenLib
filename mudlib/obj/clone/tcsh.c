#include <mudlib.h>
inherit OBJECT;
// inherit "basic/move.c";

#define ESC ""
#define CSI ESC + "["
#define CLEAR CSI + "L" + CSI + "G"


string get_player(string name);
void do_input(int c);

string sentence = "";

int
id(string str) { return str == "tcsh"; }

void
init() {
  add_action("start", "start");
}

int
start() {
  sentence = "";
  get_char("get_input", 1);
}

int
get_input(string c) {
  int size, n;

  size = strlen(c);
  for(n = 0; n < size; n++)
    do_input(c[n]);
}

void
do_input(int input) {
//  write("'"+input+"'");

  switch(input) {
  case '\t': {
    string *words;
    string name;

    words = explode(sentence, " ");
    if((name = get_player(words[sizeof(words) - 1]))) {
      words[sizeof(words)-1] = name;
      sentence = implode(words, " ");
      
    }
    write("\r"+sentence);
    break;
  }
  case 13:
  case '\r': {
    string tmp;
    
    if(sentence == "tcsh off") {
      write("Turning tcsh off.\n");
      return;
    }

    tmp = sentence;
    sentence = "";
    write("\n");
    this_player()->force_me(tmp);
    break;
  }
  case 'D': { /* The back arrow key */
    int size;

    if((size = strlen(sentence)) > 2 &&
       sentence[--size] == 91 &&
       sentence[--size] == 27) {
      sentence = sentence[0..(size - 2)];
      write("\r");
      write(CLEAR);
      write(sentence);
    }

    break;
  }
  case '\b':
  case 127: {
    int size;

    if((size = strlen(sentence)) > 2 &&
       sentence[--size] == 91 &&
       sentence[--size] == 27) {
      sentence = sentence[0..(size - 2)];
      write("\r");
      write(CLEAR);
      write(sentence);
      break;
    }
    break;
  }
  default:
    sentence = sprintf("%s%c", sentence, input);
  }
  get_char("get_input", 1);
}


string
get_player(string who) {
  object *ppl;
  int size, n;
  string *found = ({});
  ppl = users();

  write("looking for matches for '"+who+"'\n");
  for(n = 0, size = sizeof(ppl); n < size; n++) {
    string tmp;
    string name;

    name = (string)ppl[n]->query_connection()->query_real_name();
    if(sscanf(name, who+"%s", tmp)) {
      found +=({name});
    }
  }
  if(!sizeof(found))
    return 0;
  else if(sizeof(found) == 1)
    return found[0];
  for(n = 0; n < sizeof(found); n++)
    write(found[n]+" ");
  return 0;
}
