/*  -*- LPC -*-  */
// corpse.c:  Orig. by Tim 10/16/95
//   Object cloned when a living dies.
// Updated amount of time for corpse decay -> 10/23/95 Kyricc
// Updated: Now a corpse randomly becomes a Zombie -> Kyricc 10/24/95
//

#include <mudlib.h>
#include <object_types.h>

inherit OBJECT;

private static int timer;               // rotting timer

void
setup()
{
  timer = 0;
  set_heart_beat(1);                    // for rotting

  // must hold objects
  set_object_class(query_object_class() | OBJECT_CONTAINER);

  add_ids(({ "corpse", "body" }));
}

void
heart_beat()
{
  object zombie;
  int num;

  if(timer > 180) {
    num = random(21);
    if(num != 20) destruct(this_object());
      else {zombie = clone_object("/obj/clone/reinzombie.c");
            zombie->move(environment());
            destruct(this_object());}
  }
  else
  timer++;
}

string
short()
{
  switch(timer) {
  case 0..60:
    return sprintf("the corpse of %s", short_desc);
  case 61..120:
    return sprintf("the slightly stiff corpse of %s", short_desc);
  case 121..180:
    return "a rotting corpse";
  default:
    return "a decayed corpse";
  }
}

varargs
mixed
long(int flag)
{
  switch(timer) {
  case 0..60:
    long_desc = @ENDLONG
This corpse looks as though it were living only moments ago.
ENDLONG;
    break;
  case 61..120:
    long_desc = @ENDLONG
A corpse that has been deceased for a considerable amount of time.
ENDLONG;
    break;
  case 121..180:
    long_desc = @ENDLONG
A stinking, rotting corpse.
ENDLONG;
    break;
  default:
    long_desc = @ENDLONG
A decayed corpse that is beyond recognition.
ENDLONG;
  }

  return object::long(flag);
}

int get() { return 0; }
