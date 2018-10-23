/****************************************************
* Author: Michael Kucharski
* Assignment 1: reverse a file using read(), write(), lseek()
* Date: Jun 1, 2014
*****************************************************/
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  int rr;
  int infile, outfile, filesize, seek_success;
  char buffer[1];

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
  if ((outfile = open(argv[2], O_WRONLY|O_CREAT|O_TRUNC, 0644)) == -1) {
    close(infile);
    exit(EXIT_FAILURE);
  }
  
  filesize = lseek(infile, (off_t) 0, SEEK_END);

  for (int i = filesize-1; i >= 0; i--) {
    /* 
     * use lseek() to move the file pointer to the ith position of the input file.
     * To set the file pointer to a position use the SEEK_SET flag
     * in lseek().
     */
    seek_success = lseek(infile, i, SEEK_SET);
    if (seek_success < 0) { 
      exit(EXIT_FAILURE);
    }

    rr = read(infile, buffer, 1);	/* read one byte */
    if (rr != 1) {
      fprintf(stderr, "Couldn't read 1 byte [%d]\n", rr);
      exit(EXIT_FAILURE);
    }

    rr = write(outfile, buffer, 1); /* write the byte to the file */
    if (rr != 1) {
      fprintf(stderr, "Couldn't write 1 byte [%d]\n", rr);
      exit(EXIT_FAILURE);
    }
  }

  close(infile);
  close(outfile);
  exit(EXIT_SUCCESS);
}
