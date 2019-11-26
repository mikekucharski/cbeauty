/****************************************************
 * Author: Mike Kucharski
 * Assignment 3: Count the number of processes your user is running.
 * Simulate "ps -ef | grep 'mkucharski' | wc -l" using pipe(), fork(), dup2()
 * https://www.geeksforgeeks.org/pipe-system-call/
 *****************************************************/
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Process forking layout:
// P1 -> P1
//    -> P2 -> P2
//          -> P3
//
// Final Pipe Layout: P1 <-> P2 <-> P3
// Then, copy read ends of pipes to STDIN and write ends to STDOUT and replace
// contents of processes with ps, grep, wc. dup2(old_fd, new_fd)  Copies fd from
// the old location to the new location, overrites what was in new location.
int main() {
  char *username;
  int fd[2], fd2[2];
  int pid, pid2;

  // Get the current logged in user name.
  struct passwd *p = getpwuid(getuid());
  if (p == NULL) {
    perror("Unknown user.");
    exit(EXIT_FAILURE);
  }
  username = p->pw_name;

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
    // Process P2. Create second pipe and fork again.
    if (pipe(fd2) == -1) {
      perror("Failed to create second pipe");
      exit(EXIT_FAILURE);
    }
    if ((pid2 = fork()) < 0) {
      perror("fork2 failed");
      exit(EXIT_FAILURE);
    }

    if (pid2 == 0) {
      // Process P3 ('Right' process).

      // Close ends of pipes we don't care about.
      // Close both ends of first pipe (thats for P1/P2 to use).
      // Close 2nd pipe write. Leave open read of 2nd pipe.
      close(fd[0]);
      close(fd[1]);
      close(fd2[1]);

      // make STDIN point to 2nd pipe read. Then close pipe.
      dup2(fd2[0], 0);
      close(fd2[0]);

      // Replace contents of process with "wc -l".
      execlp("wc", "wc", "-l", (char *)0);
      perror("Failed to replace process with wc.");
      exit(EXIT_FAILURE);
    } else {
      // Process P2. ('Middle' Process, connects the 'Left' and 'Right')

      // Close ends of pipes we don't care about.
      // Close write of first pipe and read of second pipe.
      close(fd[1]);
      close(fd2[0]);

      // set STDIN to be 1st pipe read.
      dup2(fd[0], 0);
      close(fd[0]);

      // set STDOUT to be 2nd pipe write.
      dup2(fd2[1], 1);
      close(fd2[1]);

      // Replace contents of process with "grep 'mkuchar'".
      execlp("grep", "grep", username, (char *)0);
      perror("Failed to replace process with grep.");
      exit(EXIT_FAILURE);
    }

  } else {
    // Parent Process P1. ('Left' Process, feeds data into 1st pipe)

    // Close ends of pipes we don't care about.
    // Close read of 1st pipe. The 2nd pipe was never opened for this process.
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