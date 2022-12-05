import os

red = "\u001b[31;1m"
cyan = "\u001b[36;1m"
blue = "\u001b[34;1m"
green = "\u001b[32;1m"
white = "\u001b[37;1m"

class Node:
  def __init__(self, name, is_dir=False):
    self.name = name
    self.is_dir = is_dir
    self.inode = None
    self.contents = []
    self.path = name

  def print_contents(self):
    for node in self.contents:
      print(node.name)

  def print_inodes(self):
    for node in self.contents:
      print(node.inode, node.name)

  def print_contents_recursive(self):
    self.print_contents()
    for node in self.contents:
      if node.is_dir:
        node.print_contents_recursive()

  def add_node(self, node):
    self.contents.append(node)
    node.path = os.path.join(self.path, node.name)

  def remove_node(self, name):
    for node in self.contents:
      if node.name == name:
        self.contents.remove(node)
        return
    print(red + "File or directory does not exist.")

  def rename_node(self, old_name, new_name):
    for node in self.contents:
      if node.name == old_name:
        node.name = new_name
        return
    print(red + "File or directory does not exist.")

  def cd(self, dirname):
    for node in self.contents:
      if node.name == dirname and node.is_dir:
        return node
    return None


  def get_node(self, name):
    if name == "":
      return self
    for node in self.contents:
      if node.name == name:
        return node
    return None


  def set_path(self, path):
    self.path = path
    for node in self.contents:
      node.set_path(os.path.join(path, node.name))

  def add_node(self, node):
    self.contents.append(node)
    node.path = os.path.join(self.path, node.name)
    node.set_path(node.path)

class FileSystem:
  def __init__(self):
    self.root = Node("/")

  def ls(self, flags):
    if flags == "":
      self.root.print_contents()
    elif flags == "-i":
      self.root.print_inodes()
    elif flags == "-R":
       self.root.print_contents_recursive()

  def rm(self, filename):
    self.root.remove_node(filename)

  def mkdir(self, dirname):
    self.root.add_node(Node(dirname, True))

  def mv(self, old_name, new_name):
    self.root.rename_node(old_name, new_name)

  def touch(self, filename):
    self.root.add_node(Node(filename))

  def cd(self, dirname):
    if dirname == "..":
      parent_dir = os.path.dirname(self.root.path)
      if parent_dir != self.root.path:
        self.root = self.get_node(os.path.basename(parent_dir))
    elif dirname == "":
      self.root = self.get_node("/")
    else:
      new_root = self.root.cd(dirname)
      if new_root:
        path_parts = self.root.path.split(os.path.sep)[1:-1]
        for part in path_parts:
          new_root = new_root.get_node(part)
        self.root = new_root
      else:
        print(red + "Directory does not exist." + white)
  
  def get_node(self, name):
    return self.root.get_node(name)
  
  def pwd(self):
    print(green + self.root.path+ white)

  def help(self):
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

  def exit(self):
    return False


def main():
  print(white + "Type 'help' for a list of available commands.")
  fs = FileSystem()
  running = True
  while running:
    print()
    print(blue + fs.root.path + white + "$", end="")
    user_input = input()
    command = user_input.split(" ")[0]
    if command == "ls":
      flags = user_input.split(" ")[1] if len(  
        user_input.split(" ")) > 1 else ""
      fs.ls(flags)
    elif command == "rm":
      filename = user_input.split(" ")[1]
      fs.rm(filename)
    elif command == "mkdir":
      dirname = user_input.split(" ")[1]
      fs.mkdir(dirname)
    elif command == "mv":
      old_name = user_input.split(" ")[1]
      new_name = user_input.split(" ")[2]
      fs.mv(old_name, new_name)
    elif command == "touch":
      filename = user_input.split(" ")[1]
      fs.touch(filename)
    elif command == "cd":
      if len(user_input.split(" ")) > 1:
        dirname = user_input.split(" ")[1]
        fs.cd(dirname)
      else:
        fs.cd("")
    elif command == "pwd":
      fs.pwd()
    elif command == "help":
      fs.help()
    elif command == "exit":
      running = fs.exit()
    else:
      print(red + "Command not found. Type 'help' for a list of available commands." + white)

if __name__ == "__main__":
    main()
