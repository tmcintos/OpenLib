/*  -*- LPC -*-  */
#include <daemons.h>

int
max(int i1, int i2)
{
  if( i1 > i2 )
    return i1;
  else
    return i2;
}

// list is made up of int's and ranges
// This function will reduce a list to a new list containing the fewest
// number of int's possible (tries to group them into ranges).
// It's ugly, but it works :)
string
condense_list(string list)
{
  string* ranges = explode(replace_string(list, " ", ""), ",");
  int* numbers;

  numbers = map(filter(ranges, (: strsrch($1, '-') == -1 :)), (: to_int :));
  numbers = sort_array(numbers, 1);
  ranges = filter(ranges, (: strsrch($1, '-') != -1 :));

  // Extend out existing ranges as far as possible...
  for(int i = 0; i < sizeof(ranges); i++)
  {
    int start, end;

    sscanf(ranges[i], "%d-%d", start, end);

    while( member_array(start - 1, numbers) != -1 )
    {
      start--;
      numbers -= ({ start });
    }

    while( member_array(end + 1, numbers) != -1 )
    {
      end++;
      numbers -= ({ end });
    }

    foreach(int num in numbers)
      if( num >= start && num <= end )
	numbers -= ({ num });

    ranges[i] = sprintf("%i-%i", start, end);
  }

  // Create new ranges where possible...
  for(int i = 0; i < sizeof(numbers); i++)
  {
    int start, end;

    start = end = numbers[i];

    while( member_array(start - 1, numbers) != -1 )
    {
      start--;
      numbers -= ({ start });
    }
    
    while( member_array(end + 1, numbers) != -1 )
    {
      end++;
      numbers -= ({ end });
    }

    if( start != end )
    {
      ranges += ({ sprintf("%i-%i", start, end) });
      numbers -= ({ numbers[i] });
      i--;
    }
  }

  // Combine ranges where possible...
  // First put them in order...
  ranges = sort_array(ranges, function(string s1, string s2)
		              { 
				if( to_int(s1) > to_int(s2) )
				  return 1;
				if( to_int(s1) == to_int(s2) )
				  return 0;
				else
				  return -1;
			      });

  for(int i = 0; i < sizeof(ranges) - 1; i++)
  {
    int start, end, startnext, endnext;

    sscanf(ranges[i], "%d-%d", start, end);
    sscanf(ranges[i+1], "%d-%d", startnext, endnext);

    if( end + 1 >= startnext )
    {
      ranges[i] = sprintf("%i-%i", start, max(end, endnext));
      ranges -= ({ ranges[i+1] });
      i--;
    }
  }
  return implode(ranges + map(numbers, (: sprintf("%i", $1) :)), ",");
}

int*
expand_list(string list)
{
  string* items = explode(list, ",");
  string* ranges = filter(items, (: strsrch($1, '-') != -1 :));
  int* numbers = map(items - ranges, (: to_int :));
  
  foreach(string range in ranges)
  {
    int start, end;
    sscanf(range, "%d-%d", start, end);
    for(int i = start; i <= end; i++)
      numbers += ({ i });
  }

  return sort_array(numbers, 1);
}

int
member_list(int number, string list)
{
  string* ranges = explode(replace_string(list, " ", ""), ",");
  int* numbers = map(filter(ranges, (: strsrch($1, '-') == -1 :)),
		     (: to_int :));
  ranges = filter(ranges, (: strsrch($1, '-') != -1 :));

  if( member_array(number, numbers) != -1 )
    return 1;

  foreach(string range in ranges)
  {
    int start, end;

    sscanf(range, "%d-%d", start, end);
    if( number >= start && number <= end )
      return 1;
  }

  return 0;
}

int
max_in_list(string list)
{
  int* numbers = map(explode(replace_string(replace_string(list, " ", ""),
					    "-", ","), ","), (: to_int :));
  int max;

  foreach(int num in numbers)
    if( num > max )
      max = num;

  return max;
}

// User cmd
// Gets the user's newsrc and figures out how to read the specified group
#if 1
void
read_group(string newsgroup, mapping newsrc)
{
  int* info = NEWSSERVER->get_group_info(newsgroup);

  if( !info ||  !NEWSSERVER->can_access_group(newsgroup, this_player()) )
  {
    message("system", "You cannot access that group.\n", this_player());
    return;
  }

  // Get newsrc entry up-to-date
  if( info[0] != 1 )
    newsrc[newsgroup] = condense_list("1-"+ info[0] +","+ newsrc[newsgroup]);

  
}

void
read_loop(string newsgroup, int* info, int current_post, string input)
{
  if( input != "q")
    display(sprintf("%%^INVERSE%%^%s #%d %%^RESET%%^[npq]: ",
		    newsgroup, current_post));

  switch(input) {
  case "":
    break;
  case "q":
    return main_loop("");
  case "n":
  {
    mixed* post;

    display("%^CLS%^");
    while( !post && current_post < info[2] )
    {
      post = NEWSSERVER->get_post(newsgroup, ++current_post);
    }

    if( !post )
    {
      display("No more messages.\n");
      display(sprintf("%%^INVERSE%%^%s #%d %%^RESET%%^[npq]: ",
		      newsgroup, current_post));
      break;
    }

    this_player()->more(format_post(post),
			(: read_loop, newsgroup, info, current_post, "" :));
    return;
  }
  case "p":
  {
    mixed* post;

    display("%^CLS%^");
    while( !post && current_post > 1 )
    {
      post = NEWSSERVER->get_post(newsgroup, --current_post);
    }

    if( !post )
    {
      display("No more messages.\n");
      display(sprintf("%%^INVERSE%%^%s #%d %%^RESET%%^[npq]: ",
		      newsgroup, current_post));
      break;
    }

    this_player()->more(format_post(post),
			(: read_loop, newsgroup, info, current_post, "" :));
    return;
  }
  }
  input_to((: read_loop, newsgroup, info, current_post :));
}
#endif
