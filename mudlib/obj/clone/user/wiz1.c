/*  -*- LPC -*-  */
// wiz1.c:  Tim McIntosh  09/23/95
//   A test wizard's body for use with vt100 terminals (or equivalent)
//
#include <mudlib.h>

#define M_SHELL "shell"
#define write(x) message(M_SHELL, x, this_object());

// vt100 screen formatting stuff
#define LENGTH 25            // screen length
#define SAVEC "7"
#define RESTC "8"
#define RESET "[H[2J"
#define SCROLL(x, y) sprintf("[%d;%dr", x, y)
#define GOTO(l) sprintf("[%d;1H", l)

#define   line1 "---------------------------[ "
#define   line2 " ]------------------------------------------------"

// last line of the status bar
#define LAST_STATUS_LINE 2

// for indexing the array that represents each window in 'windows'
#define START 0
#define END 1
#define PREV 2

inherit USER_OB;

mapping windows;                // mapping of all windows
mapping assignments;            // mapping from mesg classes to window names
string curr_window;             // current window
string last_window;             // last window added (i.e. at bottom of screen)

void init_screen();
void redraw_screen();
void receive_message(string msgClass, string msg);
void write_to_window(string window, string msg);
void set_focus(string window);
int add_window(string title, int length);
int remove_window(string window);
int change_window(string window);

void reset_screen();
void save_cursor_pos();
void rest_cursor_pos();
void goto_line(int line);
void set_scroll_region(int startl, int endl);
int window_exists(string window);

void
setup(string username)
{
  ::setup(username);

  init_screen();
  add_action("cmd_add_window", "/add");
  add_action("redraw_screen", "/redraw");
  add_action("change_window", "/goto");
  add_action("remove_window", "/remove");
}

void
remove()
{
  reset_screen();
  ::remove();
}

void
receive_message(string msgClass, string msg)
{
  if(msgClass == M_SHELL) {
    ::receive_message(M_SHELL, msg);
    return;
  }
  
// if the class is bound to a window, write the message to that window
  if(member_array(msgClass, keys(assignments)) != -1) {
    write_to_window(assignments[msgClass], msg);
    return;
  }

  ::receive_message(msgClass, msg);
}

void
write_to_window(string wind, string msg)
{
  if(!window_exists(wind))
    wind = curr_window;
  if(wind == curr_window) {
    ::receive_message(M_SHELL, msg);
    return;
  }
  save_cursor_pos();
  set_focus(wind);
  ::receive_message(M_SHELL, msg);
  set_focus(curr_window);
  rest_cursor_pos();
}

void
init_screen()
{
  reset_screen();

  windows = ([]);
  assignments = (["tell" : "Communications",
		  "say"  : "Communications",
		]);

  add_window("Communications", 10);
  add_window("Main", 0);
  curr_window = "Main";
  redraw_screen();
}

void
redraw_screen()
{
  string *titles = keys(windows);
  string title;
  int tmp;

  reset_screen();
  foreach(title in titles) {
    goto_line(windows[title][START]);
    tmp = strlen(line2) - strlen(title) - 1;
    write("%^BOLD%^"line1 + title + line2[0..tmp] +"%^END%^\n"); 
  }
  write("%^L_BLUE%^");
  set_focus(curr_window);
}

int
change_window(string wind)
{
  if(!window_exists(wind))
    return notify_fail("Window does not exist.\n");
  curr_window = wind;
  set_focus(wind);
  return 1;
}

int
add_window(string title, int length)
{
  int startl, endl, last_line;

  if(last_window && arrayp(windows[last_window])) {
    last_line = windows[last_window][END];
  } else {
    last_line = LAST_STATUS_LINE;
  }

// Make room for the new window if we need to
  if(last_line + 1 + length > LENGTH) {
    int tmp, shorten_by;
    shorten_by = last_line + 1 + length - LENGTH;
    tmp = windows[last_window][END] - windows[last_window][START];
    if(shorten_by >= tmp)
      return 0;
    windows[last_window][END] -= shorten_by;
    last_line = windows[last_window][END];
  }

  startl = last_line + 1;              // first unused line

  if(!length) length = LENGTH - startl;

  endl = startl + length;              // extra line accounts for title line
  last_line = endl;

  if(endl > LENGTH) return 0;

  windows += ([title : ({ startl, endl, last_window }) ]);
  last_window = title;

  return 1;
}

int
remove_window(string wind)
{
  int endl;

  if(wind != last_window)
    return notify_fail("Can only remove last window.\n");

  if(wind == curr_window)
    curr_window = windows[wind][PREV];

  endl = windows[wind][END];

  last_window = windows[wind][PREV];
  map_delete(windows, wind);
  windows[last_window][END] = endl;

  redraw_screen();
  return 1;
}

void
set_focus(string wind)
{
  set_scroll_region(windows[wind][START]+1, windows[wind][END]);
  goto_line(windows[wind][END]);
}

int
window_exists(string wind)
{
  if(member_array(wind, keys(windows)) == -1)
    return 0;
  return 1;
}

void
goto_line(int line)
{
  write(GOTO(line));
}

void
reset_screen()
{
  write("%^END%^");
  write(RESET);
}

void
save_cursor_pos()
{
  write(SAVEC);
}

void
rest_cursor_pos()
{
  write(RESTC);
}

void
set_scroll_region(int startl, int endl)
{
  if(startl < 1) startl = 1;
  if(endl > LENGTH) endl = 25;

  write(SCROLL(startl, endl));
}

int
cmd_add_window(string arg)
{
  string title;
  int len;

  if(!arg || sscanf(arg, "%s %d", title, len) != 2)
    return notify_fail("usage: /add <windowname> <length>\n");

  if(add_window(title, len)) {
    redraw_screen();
    write("Window added.\n");
  }
  return 1;
}
