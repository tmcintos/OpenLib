inherit "basic/move.c";

#define ESC ""
#define CSI ESC + "["
#define CLEAR CSI + "L" + CSI + "G"

string sentence = "";

string get_player(string);
void do_input(int);

status
id(string str) { return str == "tcsh"; }

init() {
  add_action("start", "start");
}

status
start() {
  sentence = "";
  get_char("get_input");
}

int
get_input(string c) {
  int size, n;
  
  size = strlen(c);
  for(n = 0; n < size; n++)
    do_input(c[n]);
}

void
do_input(int in) {
/*
  write("'"+in+"'");
*/

  switch(in) {
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
  case '\r': {
    string tmp;
    
    if(sentence == "tcsh off") {
      write("Turning tcsh off.\n");
      return;
    }
    tmp = sentence;
    sentence = "";
    write("\n");
    this_player()->force_us(tmp);
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
      break;
    }
  }
/*  case '\b':
  case 127:
    write("Got a backspace!\n");
    break;
*/
  default:
    sentence = sprintf("%s%c", sentence, in);
  }
  get_char("get_input");
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

    name = (string)ppl[n]->query_real_name();
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
