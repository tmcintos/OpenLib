#include <mudlib.h>
#include <gender.h>

inherit MONSTER "/human";

void
create()
{
  object weap;
  string* vocabulary;

  ::create();

  vocabulary = explode(read_file("/log/desc"), "\n");

  for(int i=0; i < sizeof(vocabulary); i++) {
    sscanf(vocabulary[i], "%*s Descartes@%*s:%s", vocabulary[i]);
  }
  
  set_name("descartes");
  set_short("Descartes of Borg");
  set_long(@ENDLONG
The creator of the Nightmare mudlib and inspiration of mudders
everywhere.  A lengend in his own...mind?
ENDLONG);
  set_gender(GENDER_MALE);
  set_talk(85, vocabulary);
}
