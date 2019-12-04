#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

// Run this process, then run ps -eo pid,pgid,ppid,comm to see the running
// processes + process groups. Ctrl+C will kill the parent process group, but
// the child may or may not live depending on if its part of its own group or
// not.
int main(int argc, char* argv[]) {
  pid_t cpid;

  cpid = fork();

  if (cpid == 0) {
    /* CHILD */

    // set process group to itself. Comment this out to kill the child with the
    // Ctrl+C.
    setpgrp();

    // child loops forever!
    while (1)
      ;

  } else if (cpid > 0) {
    /* PARRENT */

    // set the proccess group of child
    setpgid(cpid, cpid);

    // Parrent loops forever
    while (1)
      ;

  } else {
    /*ERROR*/
    perror("fork");
    _exit(1);
  }

  return 0;
}