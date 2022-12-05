#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>

// Struct to store file or directory information
struct Node
{
  char *name;
  char *path;
  int inode;
  int is_dir;
  struct Node *next;
};

// Function to list directory contents
void list_contents(struct Node *head)
{
  printf("Listing directory contents:\n");
  struct Node *current = head;
  while (current != NULL)
  {
    printf("%s\n", current->name);
    current = current->next;
  }
}

// Function to list directory contents with inode numbers
void list_contents_inode(struct Node *head)
{
  printf("Listing directory contents with inode numbers:\n");
  struct Node *current = head;
  while (current != NULL)
  {
    printf("%s: %d\n", current->name, current->inode);
    current = current->next;
  }
}

// Function to list all files and directories in the current directory and its subdirectories
void list_contents_recursive(struct Node *head)
{
  printf("Listing directory contents recursively:\n");
  struct Node *current = head;
  while (current != NULL)
  {
    if (current->is_dir)
    {
      printf("Directory: %s\n", current->path);
      DIR *dir = opendir(current->path);
      struct dirent *ent;
      while ((ent = readdir(dir)) != NULL)
      {
        if (ent->d_type == DT_DIR)
        {
          printf("Subdirectory: %s\n", ent->d_name);
        }
        else
        {
          printf("File: %s\n", ent->d_name);
        }
      }
      closedir(dir);
    }
    current = current->next;
  }
}

// Function to remove a file or directory
void remove_node(struct Node *head, char *file)
{
  printf("Removing %s\n", file);
  struct Node *current = head;
  struct Node *previous = NULL;
  while (current != NULL)
  {
    if (strcmp(current->name, file) == 0)
    {
      if (previous == NULL)
      {
        head = current->next;
      }
      else
      {
        previous->next = current->next;
      }
      free(current);
      break;
    }
    previous = current;
    current = current->next;
  }
}

// Function to create a new directory
void make_directory(struct Node *head, char *dir_name)
{
  printf("Creating directory %s\n", dir_name);
  // Create the directory
  if (mkdir(dir_name, 0777) != 0)
  {
    printf("Error: Failed to create directory %s\n", dir_name);
    // printf("The error message is: %s\n", strerror(errno));
    return;
  }
  // Create a new Node to store the directory information
  struct Node *new_node = malloc(sizeof(struct Node));
  new_node->name = (char*)malloc(strlen(dir_name) + 1);
  strcpy(new_node->name, dir_name);
  new_node->path = (char*)malloc(strlen(dir_name) + 1);
  strcpy(new_node->path, dir_name);
  new_node->inode = 0;
  new_node->is_dir = 1;
  new_node->next = NULL;
  // Add the new Node to the linked list
  struct Node *current = head;
  while (current->next != NULL)
  {
    current = current->next;
  }
  current->next = new_node;
}

// Function to move a file or directory
void move(struct Node *head, char *source, char *destination)
{
  printf("Moving %s to %s\n", source, destination);
  // Find the source file or directory in the linked list
  struct Node *current = head;
  while (current != NULL)
  {
    if (strcmp(current->name, source) == 0)
    {
      break;
    }
    current = current->next;
  }
  if (current == NULL)
  {
    printf("Error: Source file or directory not found.\n");
    return;
  }
  // Update the path of the file or directory in the linked list
  strcpy(current->path, destination);
  // Update the name of the file or directory in the linked list
  char *name = strrchr(destination, '/');
  if (name != NULL)
  {
    name++;
    strcpy(current->name, name);
  }
  else
  {
    strcpy(current->name, destination);
  }
  // Move the file or directory
  rename(source, destination);
}

// Function to create a new file
void touch(struct Node *head, char *file_name)
{
  printf("Creating empty file %s\n", file_name);
  // Create the file
  FILE *file = fopen(file_name, "w");
  fclose(file);
  // Create a new Node to store the file information
  struct Node *new_node = malloc(sizeof(struct Node));
  new_node->name = file_name;
  new_node->path = malloc(strlen(file_name) + 1);
  strcpy(new_node->path, file_name);
  new_node->inode = 0;
  new_node->is_dir = 0;
  new_node->next = NULL;
  // Add the new Node to the linked list
  struct Node *current = head;
  while (current->next != NULL)
  {
    current = current->next;
  }
  current->next = new_node;
}

// Function to change the current directory
void change_directory(char *path)
{
  printf("Changing directory to %s\n", path);
  if (chdir(path) != 0)
  {
    printf("Error: Failed to change directory.\n");
  }
}

// Main function to run the commands
int main(void)
{
  // Create a linked list to store file and directory information
  struct Node *head = NULL;
  // Flag for whether the user wants to continue using the program
  int continue_loop = 1;
  // Loop until the user quits the program
  while (continue_loop)
  {
    printf("Enter a command (ls, ls -i, ls -R, rm, mkdir, mv, touch, cd, quit): ");
    char command[10];
    scanf("%s", command);
    if (strcmp(command, "ls") == 0)
    {
      // List the contents of the current directory
      list_contents(head);
    }
    else if (strcmp(command, "ls") == 0 && strcmp(command, "-i") == 0)
    {
      // List the contents of the current directory with inode numbers
      list_contents_inode(head);
    }
    else if (strcmp(command, "ls") == 0 && strcmp(command, "-R") == 0)
    {
      // List the contents of the current directory and its subdirectories
      list_contents_recursive(head);
    }
    else if (strcmp(command, "rm") == 0)
    {
      // Remove a file or directory
      printf("Enter the name of the file or directory to remove: ");
      char file_name[100];
      scanf("%s", file_name);
      remove_node(head, file_name);
    }
    else if (strcmp(command, "mkdir") == 0)
    {
      // Create a new directory
      printf("Enter the name of the new directory: ");
      char dir_name[100];
      scanf("%s", dir_name);
      make_directory(head, dir_name);
    }
    else if (strcmp(command, "mv") == 0)
    {
      // Move a file or directory
      printf("Enter the source file or directory: ");
      char source[100];
      scanf("%s", source);
      printf("Enter the destination: ");
      char destination[100];
      scanf("%s", destination);
      move(head, source, destination);
    }
    else if (strcmp(command, "touch") == 0)
    {
      // Create an empty file
      printf("Enter the name of the new file: ");
      char file_name[100];
      scanf("%s", file_name);
      touch(head, file_name);
    }
    else if (strcmp(command, "cd") == 0)
    {
      // Change the current working directory
      printf("Enter the new directory: ");
      char path[100];
      scanf("%s", path);
      change_directory(path);
    }
    else if (strcmp(command, "exit") == 0)
    {
      // Quit the program
      continue_loop = 0;
    }
    else
    {
      printf("Error: Invalid command.\n");
    }
  }
  return 0;
}