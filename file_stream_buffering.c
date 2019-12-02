#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void first_exit_handler() {
  printf("FIRST Exit Handler\n");
}

void second_exit_handler() {
  printf("SECOND Exit Handler\n");
}

int main() {
  // Notice how the process sleeps for 5 seconds before anything is printed to
  // the terminal (but the newline is printed first). This is because streams
  // (like stdout) are buffered until they see newline characters to avoid
  // frquent expensive system calls. When you return, the system call exit()
  // takes place which flushes all the streams during cleanup.
  printf("\nHello World!");
  sleep(3);

  printf("Hello world forcefully flushed!");
  fflush(stdout);
  sleep(3);

  // You can change the buffer policy! For example, stderr is configured to
  // immediately print (rightfully so). Lets change it:
  // int setvbuf(FILE *stream, char *buf, int mode, size_t size);
  // 1. _IONBF unbuffered : writes immediately
  // 2. _IOLBF line buffered : writes when buffer is full OR newline found
  // 3. _IOFBF fully buffered : writes when buffer is full only
  setvbuf(stderr, NULL, _IOLBF, 0);
  fprintf(stderr, "Error, but not flushed immediately!");
  sleep(3);

  // exit handers execute in reverse order of registration. second_exit_handler
  // will run before first_exit_handler.
  atexit(first_exit_handler);
  atexit(second_exit_handler);

  return 0;
}