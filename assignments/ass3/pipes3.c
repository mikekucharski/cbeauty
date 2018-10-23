/****************************************************
 * Author: Mike Kucharski
 * Assignment 3: Count the number of processes your user is running.
 * Simulate "ps -ef | grep 'mkucharski' | wc -l" using pipe(), fork(), dup()
 * https://www.geeksforgeeks.org/pipe-system-call/
 *****************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
  int fd[2], fd2[2];
  int pid, pid2;

  // Create the first pipe and fork the process. This process will share the
  // pipe files.
  if (pipe(fd) == -1) {
    perror("Failed to create first pipe");
    exit(EXIT_FAILURE);
  }
  if ((pid = fork()) < 0) {
    perror("fork1 failed");
    exit(EXIT_FAILURE);
  }

  if (pid == 0) {
    // CHILD #1

    // Note, only *PARENT*  will create second pipe and fork again.
    if (pipe(fd2) == -1) {
      perror("Failed to create second pipe");
      exit(EXIT_FAILURE);
    }
    if ((pid2 = fork()) < 0) {
      perror("fork2 failed");
      exit(EXIT_FAILURE);
    }

    if (pid2 == 0) {
      // CHILD #2 ('Right' process)

      // Close ends of pipes we don't care about.
      close(fd[0]);
      close(fd[1]);
      close(fd2[1]);

      // close 2nd pipe write.
      // make STDIN point to 2nd pipe read.
      dup2(fd2[0], 0);
      close(fd2[0]);

      // Replace contents of file with "wc -l".
      execlp("wc", "wc", "-l", (char *)0);
      perror("Failed to replace process with wc.");
      exit(EXIT_FAILURE);

    } else {
      // CHILD #1 ('Middle' Process, connects the 'Left' and 'Right')

      // Close ends of pipes we don't care about.
      close(fd[1]);
      close(fd2[0]);

      // set STDIN to be 1st pipe read.
      dup2(fd[0], 0);
      close(fd[0]);

      // set STDOUT to be 2nd pipe write.
      dup2(fd2[1], 1);
      close(fd2[1]);

      // Replace contents of process with "grep 'mkuchar'".
      execlp("grep", "grep", "mkuchar", (char *)0);
      perror("Failed to replace process with grep.");
      exit(EXIT_FAILURE);
    }

  } else {
    // PARENT ('Left' Process, feeds data into 1st pipe)

    // Close ends of pipes we don't care about.
    // (Notice, the 2nd pipe was never opened for this process.)
    close(fd[0]);

    // Set STDOUT to the write end of the 1st pipe.
    // We want to send the output of this process into the first pipe.
    dup2(fd[1], 1);
    close(fd[1]);

    // Replace contents of process with "ps -ef".
    execlp("ps", "ps", "-ef", (char *)0);
    perror("Failed to replace process with ps.");
    exit(EXIT_FAILURE);
  }

  exit(EXIT_SUCCESS);
}