// help.c
// A deamon which will call command->help or help topic and display
// the appropriate help message.
// Written by Lol on 26 Oct 1995

#include <command.h>

int main(string str) {
  if(!str) {
    // general help page here //
  }
  if(file_size("/cmd/player/"+str + ".c") >=0 ) {
    call_other(str+".c", "help");
  }
  return 1;
}
