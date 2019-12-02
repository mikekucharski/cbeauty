#include <stdio.h>
#include <stdlib.h>

/**
 * To do processing on any unix file (pipe, socket, regular file, etc) you have
 * a choice of raw file descriptor integer or FILE* stream object. Under the
 * covers streams are file descriptors, so you can get the underlying fd and do
 * low level operations on the strema if you want. See
 * https://www.gnu.org/software/libc/manual/html_node/Streams-and-File-Descriptors.html#Streams-and-File-Descriptors
 * for more details.
 */
int main(int argc, char* argv[]) {
  char readString[60];
  // See File Stream modes from the man pages: man fopen()
  /**
   * Note, fopen(), fprintf(), fscanf(), fgets(), fread(), fwrite() are library
   * functions. fprintf/fscanf are for formatted strings, where fread() and
   * fwrite() are for arbitrary buffered data. The 'f' in front means you can
   * specify a stream as the first argument. The same methods without 'f' in
   * front defualt to input/output streams (ie. stdin, stdout)
   */
  FILE* stream = fopen("/tmp/cbeauty.txt", "w+");
  if (stream == NULL) {
    fprintf(stderr, "Failed to open file.\n");
    return 1;
  }
  fprintf(stream, "Hello World!\n");
  fclose(stream);

  // Read from tmp file.
  FILE* readStream = fopen("/tmp/cbeauty.txt", "r");
  if (readStream == NULL) {
    fprintf(stderr, "Failed to open file for read.\n");
    return 1;
  }
  while (fgets(readString, 60, readStream) != NULL) {
    printf("Read in string from temp file: %s", readString);
  }
  fclose(readStream);
  return 0;
}