#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
  // arguments for ls, will run: ls  -l /
  // Note, argv array must be null terminated!
  char* ls_args[4] = {"ls", "-l", "/", NULL};
  pid_t c_pid, pid;
  int status;

  c_pid = fork();

  if (c_pid == 0) {
    /* CHILD */

    printf("Child: executing ls\n");

    // execute ls
    execvp(ls_args[0], ls_args);
    // only get here if exec failed
    perror("execve failed");
  } else if (c_pid > 0) {
    /* PARENT */

    if ((pid = wait(&status)) < 0) {
      perror("wait failed");
      _exit(1);
    }
    printf("Parent: finished with status: %d\n", status);
  } else {
    perror("fork failed");
    _exit(1);
  }

  return 0;
}