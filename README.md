# hfsh24 - Horned Frog Shell Version 2

## Overview
`hfsh24` is a simple Unix shell implemented as a part of an educational project for learning about the Linux programming environment, process management, and shell functionalities. This command line interpreter (CLI), designed for educational purposes, simulates basic behaviors of traditional Unix shells but with simplified functionality and educational extensions for process creation and management.

## Features

### Interactive and Batch Modes
- **Interactive Mode:** Run `hfsh24` without arguments to enter an interactive mode where commands can be entered one at a time.
- **Batch Mode:** Run `hfsh24` with a single argument (e.g., `./hfsh24 batch.txt`) to process commands from a batch file.

### Command Execution
- Executes user commands by creating a new process for each command, except for built-in commands.
- Built-in commands include:
  - `exit`: Exits the shell.
  - `cd`: Changes the current directory.
  - `path`: Sets the search path for executable commands.

### Redirection
- Supports redirection of both standard output and standard error to a file using the `>` operator, allowing users to direct output to files instead of the terminal.

### Parallel Commands
- Supports executing commands in parallel using the `&` operator, enabling multiple commands to run simultaneously and independently.

## Installation and Usage

### Prerequisites
- Linux environment with GCC compiler
- Standard C/C++ library

### Compiling
- Run the MakeFile

### Running
- Run ./hfsh24 for interactive mode
- Run ./hfsh24 [file] where the file is a .txt file containing the commands you wish to execute
