string
user_cwd(string name)
{
   return ("/u/" + name[0..0] + "/" + name);
}

string user_path(string name)
{
   return (user_cwd(name) + "/");
}

/*
// Thanks to Huthar for resolve_path.
// Rewrite by Symmetry 5/4/95
// 09/16/95  Tim McIntosh:  Fixed this so it actually works right :)
*/

string resolve_path(string curr, string newer) {
  int i, j, size;
  string *tmp;
    
  switch(newer) {
  case 0: 
  case ".":
    return curr;

  case "~":
    return user_path((string)this_player()->query_name());
	
  case "here":
    return file_name(environment(this_user()))+".c";
	
  default:
    if (newer[0..1] == "~/") {
      newer = user_path((string)this_player()->query_name()) + newer[2..];
    } else {
      switch(newer[0]) {
      case '~':
	{
	  i = strsrch(newer, '/');
	  if (i < 0)
	    newer = user_path(newer[1..]);
	  else
	    newer = user_path(newer[1..i-1]) + newer[i..];
	  break;
	}
      case '/': break;
      default: newer = curr + "/" + newer;
      }
    }
    
    if (newer[<1] != '/') newer += "/";

/*  Figure out .. and .'s in the path  */
    size = sizeof(tmp = regexp(explode(newer, "/"), "."));
    
    i = j = 0;
    
    while (i < size) {
      switch(tmp[i]) {
      case "..":
	if (j){
	  while (j-- && !tmp[j]);
	  if (j >= 0) tmp[j] = 0;
	  else j++;
	}
      case ".":
	tmp[i++] = 0;
	break;
	
      default:
	j = ++i;
	break;
      }
    }
    return "/"+implode(tmp, "/");
  }
}
