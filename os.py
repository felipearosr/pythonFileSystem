import datetime
from os import listdir, mkdir, remove
import sys

red = "\u001b[31;1m"
cyan = "\u001b[36;1m"
blue = "\u001b[34;1m"
green = "\u001b[32;1m"
white = "\u001b[37;1m"

root = {
  "type": "dir",
  "name": "root",
  "children": []
}

cwd = root

def get_path(node):
  if node == root:
    return "/"
  else:
    return get_path(node["parent"]) + node["name"] + "/"

def ls(options):
  if "-i" in options:
    for child in cwd["children"]:
      print(child["inode"], child["name"])
  elif "-R" in options:
    # Print current directory
    print(cwd["name"] + white)
    for child in cwd["children"]:
      if child["type"] == "dir":
        # Print directory name
        print(blue + "./" + child["name"]  + ":")
        # Change directory to child
        cd(child["name"])
        # Recursively call ls function
        ls(options)
        # Change directory back to parent
        cd("..")
      else:
        print(child["name"])
  else:
    for child in cwd["children"]:
      print(child["name"])

def rm(name):
  for i, child in enumerate(cwd["children"]):
    if child["name"] == name:
      cwd["children"].pop(i)
      return
  print(red + "rm: cannot remove '" + name + "': No such file or directory" + white)

def mv(old_name, new_name):
  for i, child in enumerate(cwd["children"]):
    if child["name"] == old_name:
      child["name"] = new_name
      return
  print(red + "mv: cannot move '" + old_name + "': No such file or directory" + white)

def mkdir(name):
  for child in cwd["children"]:
    if child["name"] == name:
      print(red + "mkdir: cannot create directory '" + name + "': File exists" + white)
      return
  cwd["children"].append({
    "type": "dir",
    "name": name,
    "children": [],
    "parent": cwd
  })

def cd(path):
  global cwd
  if not path:
    print(red + "cd: missing operand" + white)
    return
  if path == "..":
    if cwd == root:
      return
    for child in cwd["parent"]["children"]:
      if child["name"] == cwd["name"]:
        cwd = cwd["parent"]
        return
  elif path == "/":
    cwd = root
    return
  else:
    for child in cwd["children"]:
      if child["name"] == path:
        if child["type"] == "dir":
          cwd = child
          return
        else:
          print(red + "cd: cannot cd into '" + path + "': Not a directory" + white)
          return
    print(red + "cd: cannot cd into '" + path + "': No such file or directory" + white)

def touch(name):
  next_inode = 1  # Set initial inode value
  for child in cwd["children"]:
    if child["name"] == name:
      print(red + "touch: cannot touch '" + name + "': File exists" + white)
      return
    else:
      child["last_accessed"] = datetime.datetime.now()
      child["last_modified"] = datetime.datetime.now()
      # Check if child has inode attribute
      if "inode" in child:
        # Set inode value for next file
        next_inode = max(next_inode, child["inode"] + 1)
      return
  cwd["children"].append({
    "type": "file",
    "name": name,
    "children": [],
    "inode": next_inode,  # Set inode attribute for new file
    "last_accessed": datetime.datetime.now(),
    "last_modified": datetime.datetime.now()
  })

def pwd():
  print(green + get_path(cwd))

def help():
  print(white + "Available commands:")
  print(blue + "  ls                " + white + "lists contents of current directory")
  print(blue + "  ls -i             " + white + "lists i-nodes of current directory")
  print(blue + "  ls -R             " + white + "lists contents of current directory and all subdirectories recursively")
  print(blue + "  rm <name>         " + white + "removes a file or directory")
  print(blue + "  mkdir <name>      " + white + "creates a new directory")
  print(blue + "  touch <name>      " + white + "creates a new file")
  print(blue + "  exit              " + white + "exits the file system simulator")
  print(blue + "  mv <src> <dst>    " + white + "renames a file or directory")
  print(blue + "  cd <path>         " + white + "changes the current directory")
  print(blue + "  help              " + white + "displays this message")
  print(blue + "  pwd               " + white + "prints the current directory path (e.g. '/home/user/.../lab3')")

def main():
  while True:
    command = input(blue + "~" + get_path(cwd) + white + "$ ")
    if command == "exit":
      sys.exit()
    elif command == "help":
      help()
    elif command == "pwd":
      pwd()
    elif command.startswith("ls"):
      ls(command.split(" ")[1:])
    elif command.startswith("rm"):
      rm(command.split(" ")[1])
    elif command.startswith("mv"):
      mv(command.split(" ")[1], command.split(" ")[2])
    elif command.startswith("mkdir"):
      mkdir(command.split(" ")[1])
    elif command.startswith("cd"):
      cd(command.split(" ")[1])
    elif command.startswith("touch"):
      touch(command.split(" ")[1])
    else:
      print("Command not found")

if __name__ == "__main__":
  main()