/*  -*- LPC -*-  */
// time.c:  Tim McIntosh
//          Some useful time functions.  Also need to #include <time.h>
//          so you can use the #defines in there.

#include <time.h>

string
get_date(int time, int which_part)
{
  string tmp, tmp2;

  switch(which_part) {
  case DATESTR_ALL:
    return ctime(time);

  case DATESTR_DATE:
    sscanf(ctime(time), "%*s %s %d %*s", tmp, tmp2);
    return tmp +" "+ tmp2;

  case DATESTR_TIME:
    sscanf(ctime(time), "%*s %*s %*s %s %*s", tmp);
    return "" + tmp;

  case DATESTR_YEAR:
    sscanf(ctime(time), "%*s %*s %*d %*s %d", tmp);
    return "" + tmp;

  case DATESTR_SHORT:
    sscanf(ctime(time), "%*s %s:%s:%*s %*s", tmp, tmp2);
    return tmp +":"+ tmp2;

  case DATESTR_DAY:
    sscanf(ctime(time), "%s %*s", tmp);
    switch(tmp) {
    case "Sun":
      return "Sunday";
    case "Mon":
      return "Monday";
    case "Tue":
      return "Tuesday";
    case "Wed":
      return "Wednesday";
    case "Thu":
      return "Thursday";
    case "Fri":
      return "Friday";
    case "Sat":
      return "Saturday";
    default:
      return "Someday";
    }

  case DATESTR_MONTH:
    sscanf(ctime(time), "%*s %s %*s", tmp);
    switch(tmp) {
    case "Jan":
      return "January";
    case "Feb":
      return "February";
    case "Mar":
      return "March";
    case "Apr":
      return "April";
    case "May":
      return "May";
    case "Jun":
      return "June";
    case "Jul":
      return "July";
    case "Aug":
      return "August";
    case "Sep":
      return "September";
    case "Oct":
      return "October";
    case "Nov":
      return "November";
    case "Dec":
      return "December";
    default:
      return "Janember";
    }

  default:
    return ctime(time);
  }
}
