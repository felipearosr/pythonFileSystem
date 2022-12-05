#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <map>
// Forward declarations
class Node;
class Directory;
class File;

// A map that maps file and directory names to nodes
typedef std::unordered_map<std::string, Node *> NodeMap;

// Base class for a node (file or directory) in the file system
class Node
{
public:
  Node(const std::string &name) : name_(name) {}
  virtual ~Node() {}

  // Returns the name of the node
  std::string name() const { return name_; }

  // Returns the type of the node (either "file" or "directory")
  virtual std::string type() const = 0;

  // Returns the size of the node in bytes
  virtual size_t size() const = 0;

  // Returns a map of the children of this node
  const NodeMap &children() const { return children_; }

  // The name of the node
  std::string name_;

private:
  // A map of the children of this node
  NodeMap children_;
};


// A class for a directory in the file system
class Directory : public Node
{
public:
  Directory(const std::string &name) : Node(name), children_() {}

  // Adds a child node to the directory
  void add(Node *child) { children_[child->name()] = child; }

  // Removes a child node from the directory
  void remove(const std::string &name) { children_.erase(name); }

  // Returns the type of the node (always "directory")
  std::string type() const override { return "directory"; }

  // Returns the size of the directory in bytes (always 0)
  size_t size() const override { return 0; }

  // Returns a map of the children of this directory
  const NodeMap &children() const { return children_; }

  // Returns the parent of this directory, or nullptr if it has no parent
  Directory *parent() const { return parent_; }

  // Sets the parent of this directory
  void set_parent(Directory *parent) { parent_ = parent; }

private:
  NodeMap children_;
  Directory *parent_ = nullptr;
};

// A class for a file in the file system
class File : public Node
{
public:
  File(const std::string &name, size_t size) : Node(name), size_(size) {}

  // Returns the type of the node (always "file")
  std::string type() const override { return "file"; }

  // Returns the size of the file in bytes
  size_t size() const override { return size_; }

private:
  size_t size_;
};

// The root directory of the file system
Directory root("/");

// The current working directory
Directory *cwd = &root;

// Declare the FileSystem namespace
// Declare the FileSystem namespace
namespace FileSystem
{
  // Declare the NodeMap type
  // Make NodeMap inherit from std::map and specify the key and value types as template parameters
  using NodeMap = std::map<std::string, Entry*>;

  // Declare the Entry class
  class Entry
  {
    public:
      // Constructor
      Entry(const std::string &name) : name_(name) {}

      // Get the name of the entry
      const std::string& name() const { return name_; }

      // Get the parent directory of the entry
      Directory* parent() const { return parent_; }

      // Set the parent directory of the entry
      void set_parent(Directory *parent) { parent_ = parent; }

    private:
      std::string name_;
      Directory *parent_;
  };

  // Lists the files and directories in the given directory
  void ls(const Directory &dir)
  {
    // Import the map and Entry types into the current namespace
    using std::map;

    // Get the map of children for the given directory
    // Use the fully qualified name of the NodeMap type to refer to it
    const map<std::string, Entry*>& children = dir.children();

    // Print the names of the files and directories in the given directory
    for (auto& child : children)
    {
      std::cout << child.first << " ";
    }
    std::cout << std::endl;
  }
}


// Lists the files and directories in the given directory,
// including the i-node number of each file and directory
void ls_i(const Directory &dir)
{
  for (const auto &[name, node] : dir.children())
  {
    std::cout << node << " " << name << " " << node->type() << std::endl;
  }
}

// Lists the files and directories in the given directory recursively
void ls_R(const Directory &dir, const std::string &indent = "")
{
  for (const auto &[name, node] : dir.children())
  {
    std::cout << indent << name << " " << node->type() << std::endl;
    if (node->type() == "directory")
    {
      ls_R(*static_cast<Directory *>(node), indent + "  ");
    }
  }
}

// Removes a file or directory from the file system
void rm(const std::string &name)
{
  // Check if the node exists
  if (cwd->children().count(name) == 0)
  {
    std::cout << "rm: cannot remove '" << name << "': No such file or directory" << std::endl;
    return;
  }

  // Get the node to remove
  Node *node = cwd->children().at(name);

  // Check if the node is a directory
  if (node->type() == "directory")
  {
    Directory *dir = static_cast<Directory *>(node);

    // Check if the directory is empty
    if (dir->children().size() > 0)
    {
      std::cout << "rm: cannot remove '" << name << "': Directory not empty" << std::endl;
      return;
    }
  }

  // Remove the node
  cwd->remove(name);
}

// Makes a new directory in the file system
void mkdir(const std::string &name)
{
  // Check if the directory already exists
  if (cwd->children().count(name) > 0)
  {
    std::cout << "mkdir: cannot create directory '" << name << "': File exists" << std::endl;
    return;
  }

  // Create a new directory and add it to the current working directory
  Directory *dir = new Directory(name);
  dir->set_parent(cwd);
  cwd->add(dir);
}

// Exits the program
void exit()
{
  std::exit(0);
}

// Moves a file or directory to a new location
void mv(const std::string &src, const std::string &dst)
{
  // Check if the source file or directory exists
  if (cwd->children().count(src) == 0)
  {
    std::cout << "mv: cannot move '" << src << "': No such file or directory" << std::endl;
    return;
  }

  // Get the node to move
  Node *node = cwd->children().at(src);

  // Check if the destination exists
  if (cwd->children().count(dst) != 0)
  {
    std::cout << "mv: cannot move '" << src << "' to '" << dst << "': File or directory already exists" << std::endl;
    return;
  }

  // Create a new node with the destination name
  Node *new_node = nullptr;
  if (node->type() == "file")
  {
    new_node = new File(dst, static_cast<File *>(node)->size());
  }
  else if (node->type() == "directory")
  {
    new_node = new Directory(dst);
  }

  // Add the new node to the file system
  cwd->add(new_node);

  // Remove the old node from the file system
  cwd->remove(src);
}

// Creates an empty file
void touch(const std::string &name)
{
  // Check if the file already exists
  if (cwd->children().count(name) > 0)
  {
    std::cout << "touch: cannot create file '" << name << "': File already exists" << std::endl;
    return;
  }

  // Create the new file
  File *file = new File(name, 0);
  cwd->add(file);
}

// Changes the current working directory
void cd(const std::string& path) {
  if (path == "..") {
    // Go up one directory level if the path is ".."
    if (cwd == &root) {
      std::cout << "cd: cannot change directory to '..': No such file or directory" << std::endl;
      return;
    }
    cwd = static_cast<Directory*>(cwd->parent());
    return;
  } else if (path == "...") {
    // Go up two directory levels if the path is "..."
    if (cwd == &root) {
      std::cout << "cd: cannot change directory to '...': No such file or directory" << std::endl;
      return;
    }
    if (cwd->parent() == nullptr || cwd->parent()->parent() == nullptr) {
      std::cout << "cd: cannot change directory to '...': No such file or directory" << std::endl;
      return;
    }
    cwd = static_cast<Directory*>(cwd->parent()->parent());
    return;
  } else if (path == "/") {
    // Change to the root directory if the path is "/"
    cwd = &root;
    return;
  }

  // Check if the path exists in the current working directory
  if (cwd->children().count(path) == 0) {
    std::cout << "cd: cannot change directory to '" << path << "': No such file or directory" << std::endl;
    return;
  }

  // Get the node at the given path
  Node* node = cwd->children().at(path);

  // Check if the path is a directory
  if (node->type() != "directory") {
    std::cout << "cd: cannot change directory to '" << path << "': Not a directory" << std::endl;
    return;
  }

  // Set the current working directory to the given path
  cwd = static_cast<Directory*>(node);
}

int main()
{
  std::cout << "Welcome to the file system!" << std::endl;
  std::cout << "Enter a command (or 'help' for a list of commands):" << std::endl;

  while (true)
  {
    // Print the current working directory
    std::cout << cwd->name() << "> ";

    // Read the command from the user
    std::string input;
    std::getline(std::cin, input);

    // Use a stringstream to extract the command and the flag
    std::stringstream ss(input);
    std::string command;
    ss >> command;

    // Execute the command
    if (command == "help")
    {
      std::cout << "Available commands:" << std::endl;
      std::cout << "  ls                 Lists the files and directories in the current directory" << std::endl;
      std::cout << "  ls -i              Lists the files and directories in the current directory, including the i-node number of each file and directory" << std::endl;
      std::cout << "  ls -R              Lists the files and directories in the current directory recursively" << std::endl;
      std::cout << "  mkdir <name>       Creates a new directory with the given name" << std::endl;
      std::cout << "  touch <name>       Creates a new file with the given name" << std::endl;
      std::cout << "  rm <name>          Removes the file or directory with the given name" << std::endl;
      std::cout << "  mv <src> <dst>     Moves the file or directory at the given source path to the destination path" << std::endl;
      std::cout << "  cd <path>          Changes the current working directory to the given path" << std::endl;
      std::cout << "  exit               Exits the file system" << std::endl;
    }
    else if (command == "ls")
    {
      ls(*cwd);
    }
    // Check if the next token in the stringstream is "-i" or "-R"
    else if (command == "ls" && (ss.peek() == '-'))
    {
      std::string flag;
      ss >> flag;
      if (flag == "-i")
      {
        ls_i(*cwd);
      }
      // Add this else if statement to handle the "ls -R" command
      else if (flag == "-R")
      {
        ls_R(*cwd);
      }
      else
      {
        std::cout << "ls: invalid option -- '" << flag << "'" << std::endl;
      }
    }
    else if (command == "mkdir")
    {
      std::string name;
      std::cin >> name;
      mkdir(name);
    }
    else if (command == "touch")
    {
      // Create a new file
      std::string name;
      std::cin >> name;
      touch(name);
    }
    else if (command == "mv")
    {
      // Move a file or directory
      std::string src;
      std::string dst;
      std::cin >> src >> dst;
      mv(src, dst);
    }
    else if (command == "rm")
    {
      // Remove a file or directory
      std::string name;
      std::cin >> name;
      rm(name);
    }
    else if (command == "exit")
    {
      // Exit the program
      break;
    }
    else
    {
      std::cout << "Invalid command" << std::endl;
    }
  }
  return 0;
}