string cwd = "";

string
eventReadFile(string fname)
{
  fname = absolute_path(cwd, fname);
  if(file_exists(fname))
    return read_file(fname);
  else
    return "/* -*- LPC -*- */";
}

string
eventWriteFile(string fname, string text)
{
  fname = absolute_path(cwd, fname);

  if(file_exists(fname)) {
    rm(fname+"~");
    rename(fname, fname+"~");
  }
  if(!write_file(fname, text))
    return "50 Failed to write file" + fname;
  else
    return "110 File "+ fname +" written.";
}

string
eventCommand(string cmd, string arg)
{
  switch(cmd) {
  case "ls":
    if(!arg)
      arg = cwd;
    else
      arg = absolute_path(cwd, arg);

    if(arg[<1] != '/') arg += "/";
    return arg +" "+ implode(get_dir(arg), " ");
  case "cd":
    if(!arg) arg = "/u/k/kyricc";
    else arg = absolute_path(cwd, arg);
    if(!directory_exists(arg))
      return "CMD cd failed.";
    cwd = arg;
    return "CMD cd succeeded.";
  case "update":
    if(!arg) return 0;
    if("/cmd/wiz/update"->update_file(absolute_path(cwd, arg), 1, 1))
      return "CMD update succeeded.";
    else
      return "CMD update failed.";
  case "pwd":
    return "CWD is "+ cwd +"/";
  case "mkdir":
    arg = absolute_path(cwd, arg);
    if(!mkdir(arg)) return "CMD mkdir failed.";
    return "CMD mkdir succeeded.";
  case "rmdir":
    arg = absolute_path(cwd, arg);
    if(!rmdir(arg)) return "CMD rmdir failed.";
    return "CMD rmdir succeeded.";
  }
  return "CMD "+ cmd +" failed."; 
}
