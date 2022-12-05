#!/bin/bash

# Keep prompting the user for commands until they enter "exit"
while true; do
  # Prompt the user for a command
  echo -n "Enter a command: "
  read command

  # Check if the user entered "exit"
  if [ "$command" == "exit" ]; then
    # If the user entered "exit", exit the current shell
    exit
  else
    # If the user entered a different command, run the command
    $command
  fi
done
