/*
* Intro to pipes - Pipes are a form of interprocess communication (IPC). They 
* are useful right before fork() to allow 2 processes to talk to each other. 
* However, we will read and write from onein the same process in this silly 
* example.
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MSG_SIZE 12
char* message = "hello, world";

int main() {
  int fd[2];
  int bytes_written, bytes_read;
  char buffer[MSG_SIZE];
  
  // create pipe (opens 2 files, 1 for reading and 1 for writing).
  // Pipes are NOT bidrectional. Can only write to fd[1] and read from fd[0].
  if (pipe(fd) == -1) {
    printf("Failed to create the pipe.\n");
    exit(EXIT_FAILURE);
  }

  // write message into input of pipe.
  bytes_written = write(fd[1], message, MSG_SIZE);
  if (bytes_written < 0) {
    printf("Failed to write to the input of the pipe.\n");
    exit(EXIT_FAILURE);
  }

  // read message from output of pipe, store into buffer.
  bytes_read = read(fd[0], buffer, MSG_SIZE);
  if (bytes_read < 0) {
    printf("Failed to read from the output of the pipe.\n");
    exit(EXIT_FAILURE);
  }

  printf("Read from the output of the pipe and got: %s\n", buffer);
  close(fd[0]);
  close(fd[0]);
  exit(EXIT_SUCCESS);
}