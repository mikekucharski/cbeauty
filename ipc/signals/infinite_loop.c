#include <stdio.h>
#include <stdlib.h>
#include <signal.h> /* for signal() */

/* hello_loop.c */
void hello(int signum) {
  printf("Signal handler for signal number: %d\n", signum);
}

int main() {
  // Note: changing a signal handler can happen at any time
  // signal(SIGINT, SIG_IGN); // Constant for "ignore" handler.
  // signal(SIGINT, SIG_DFL); // Constant for "replace with default" handler.

  // Handle SIGINT with hello
  signal(SIGINT, hello);

  //loop forever!
  while(1);
}