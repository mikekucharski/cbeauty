#include <stdlib.h>
#include <unistd.h>

// Note: after running use ps -ef | grep zombie to see zombie processes. Parent
// never dies so children do not completely terminate even after _exit() because
// they are waiting for parent process to call wait(). If the parent process
// dies, the parent of orphaned processes becomes init (this is intentional for
// deamon processes).
int main() {
  int i;
  for (i = 0; i < 10; i++) {
    if (fork() == 0) {  // create a child
      _exit(0);         // and exit
    }
  }

  while (1)
    ;  // run forever
}