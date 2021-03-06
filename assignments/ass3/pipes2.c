/*
 * Pipes with fork(). The pipe files get cloned but there is still 1 pipe.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MSG_SIZE 12
char* message = "hello, world";

int main() {
  int fd[2];
  int pid;
  int bytes_written, bytes_read;
  char buffer[MSG_SIZE];

  // create pipe (opens 2 files, 1 for reading and 1 for writing).
  // Pipes are NOT bidrectional. Can only write to fd[1] and read from fd[0].
  if (pipe(fd) == -1) {
    printf("Failed to create the pipe.\n");
    exit(EXIT_FAILURE);
  }

  // fork the process into 2, the pipe files will be copied with the file
  // descriptor table. Parent process gets returned the ID of the child process,
  // child process return value is 0.
  if ((pid = fork()) < 0) {
    perror("fork failed");
    exit(EXIT_FAILURE);
  }

  if (pid == 0) {
    // CHILD (2nd process)

    // Close read end of the pipe because we don't need it.
    close(fd[0]);

    // write message into input of pipe.
    bytes_written = write(fd[1], message, MSG_SIZE);
    if (bytes_written < 0) {
      printf("Failed to write to the input of the pipe.\n");
      exit(EXIT_FAILURE);
    }
    close(fd[1]);
  } else {
    // PARENT (main process)

    // Close write end of the pipe because we don't need it.
    close(fd[1]);
    // read message from output of pipe, store into buffer.
    bytes_read = read(fd[0], buffer, MSG_SIZE);
    if (bytes_read < 0) {
      printf("Failed to read from the output of the pipe.\n");
      exit(EXIT_FAILURE);
    }
    printf("Child read from the output of the pipe and got: %s\n", buffer);
    close(fd[0]);
  }

  exit(EXIT_SUCCESS);
}
