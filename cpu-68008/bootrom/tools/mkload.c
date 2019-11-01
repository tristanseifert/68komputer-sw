/**
 * Sends a binary file over a serial port.
 *
 * Compile with: clang mkload.c -o mkload
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

/**
 * Sets up the serial port.
 */
int setUpSerial(FILE *port, int baud) {
  int fd = fileno(port);

  // get existing terminal parameters
  struct termios tty;
  memset(&tty, 0, sizeof tty);

  if(tcgetattr(fd, &tty) != 0) {
    perror("tcgetattr() failed:");
    return errno;
  }

  // set the speed
  cfsetospeed(&tty, B9600);
  cfsetispeed(&tty, B9600);

  // use 8 bit characters
  tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;

  tty.c_iflag &= ~IGNBRK;         // disable break processing
  tty.c_lflag = 0;                // no signaling chars, no echo,
                                  // no canonical processing
  tty.c_oflag = 0;                // no remapping, no delays

  // read always blocks
  tty.c_cc[VMIN]  = 1;
  tty.c_cc[VTIME] = 0;

  // disable XON/XOFF
  tty.c_iflag &= ~(IXON | IXOFF | IXANY);

  // ignore modem controls and enable read
  tty.c_cflag |= (CLOCAL | CREAD);
  // disable parity
  tty.c_cflag &= ~(PARENB | PARODD);
  // tty.c_cflag |= 0;
  tty.c_cflag &= ~CSTOPB;
  tty.c_cflag &= ~CRTSCTS;

  if(tcsetattr(fd, TCSANOW, &tty) != 0) {
    perror("tcsetattr() failed:");
    return errno;
  }

  return 0;
}

/**
 * Waits for an acknowledgement.
 */
int waitForAck(FILE *port) {
  size_t read;
  uint8_t buffer[4];

  memset(&buffer, 0, sizeof(buffer));

  // wait to read a period
  read = fread(&buffer, 1, 1, port);

  if(read != 1) {
    return -1;
  }

  // is it a period?
  if(buffer[0] != '.') {
    return -1;
  }

  // success
  return 0;
}


/**
 * Main entry point
 */
int main(int argc, char **argv) {
  size_t read;

  uint8_t buffer[4];

  // ensure arg count
  if(argc != 4) {
    fprintf(stderr, "usage: %s [binary path] [load address] [serial port]\n", argv[0]);
    return -1;
  }

  // try to open the serial port
  FILE *port = fopen(argv[3], "wb");

  // int baud = strtoul(argv[4], NULL, 0);
  read = setUpSerial(port, -1);

  if(read != 0) {
    fprintf(stderr, "Failed to open serial port: %zu\n", read);
    return -1;
  }

  // try to open the input file
  FILE *in = fopen(argv[1], "rb");

  if(in == NULL) {
    perror("Failed to open input file");
    return -1;
  }

  // get the file size
  fseek(in, 0L, SEEK_END);
  size_t inSz = ftell(in);
  fseek(in, 0L, SEEK_SET);

  // write the load address (in hex)
  uint32_t loadAddr = strtoul(argv[2], NULL, 0);
  fprintf(port, "%08x", loadAddr);

  // wait for ack
  if(waitForAck(port) != 0) {
    fprintf(stderr, "Failed to get acknowledgement for load address\n");
    return -1;
  }

  // then, print the byte count
  fprintf(port, "%08zx", inSz);

  // wait for ack
  if(waitForAck(port) != 0) {
    fprintf(stderr, "Failed to get acknowledgement for byte count\n");
    return -1;
  }

  // write each of the bytes
  while(!feof(in)) {
    // read a byte
    read = fread(&buffer, 1, 1, in);

    // if(read != 1) {
    //   fprintf(stderr, "fread returned invalid value: %zd\n", read);
    //   return -1;
    // }

    // secrete the data
    fprintf(port, "%02x", buffer[0]);

    // wait for ack
    if(waitForAck(port) != 0) {
      fprintf(stderr, "Failed to get acknowledgement during data tx\n");
      return -1;
    }
  }

  // clean up
  fprintf(port, "\n");

  fclose(in);
  fclose(port);
}
