/****************************************************
 * Author: Mike Kucharski
 * Assignment 2: copy an exe using read(), write()
 * Date: June 6, 2014
 ****************************************************/
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  char buffer[256];
  int infile, outfile, expected_bytes, bytes_read, rr;

  if (argc != 3) {
    fprintf(stderr, "USAGE: %s inputFile outputFile.\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  // Open the first file in read only mode.
  if ((infile = open(argv[1], O_RDONLY)) == -1) {
    exit(EXIT_FAILURE);
  }

  // Open the second file in write only mode.
  // Create it if it doesn't exist.
  // If it does exist truncate it to size 0.
  if ((outfile = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0755)) == -1) {
    close(infile);
    exit(EXIT_FAILURE);
  }

  /*
   * 1) Start loop.
   * 2) Read from input file into the buffer (more than 1 character at a time).
   * 3) Write from the buffer into the output file.
   */
  expected_bytes = sizeof(buffer);
  for (;;) {
    bytes_read = read(infile, buffer, expected_bytes);
    if (bytes_read != expected_bytes) {
      printf("Tried reading %d bytes but got %d.\n", expected_bytes,
             bytes_read);
    }

    if (bytes_read < 0) {
      printf("bytes_read < 1. The read had an unexpected failure.\n");
      break;
    } else if (bytes_read == 0) {
      printf("Read 0 bytes, likely means EOF.\n");
      break;
    }

    rr = write(outfile, buffer, bytes_read);
    if (rr != bytes_read) {
      fprintf(stderr, "Couldn't write %d byte [%d]\n", bytes_read, rr);
      exit(EXIT_FAILURE);
    }
  }

  close(infile);
  close(outfile);
  exit(EXIT_SUCCESS);
}
