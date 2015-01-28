#include "uart.h"
#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>

#define PRINT_TAG "UART"

struct termios config;

static void setupSerialDevice(struct termios *cfg) {
  cfg->c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR | 
                    PARMRK | INPCK | ISTRIP | IXON | IXOFF);
  
  cfg->c_oflag &= ~(OCRNL | ONLCR | ONLRET | ONOCR | OFILL | OPOST);

  cfg->c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);

  cfg->c_cflag &= ~(CSIZE | PARENB | CRTSCTS);

  cfg->c_cflag |= CS8 | CLOCAL;

  cfg->c_cc[VMIN] = 1;
  cfg->c_cc[VTIME] = 0;

  if (cfsetispeed(cfg, B115200) < 0 || cfsetospeed(cfg, B115200)) {
    printf("Failed to set speed");
  }
}

int32_t uart_init() {
  int32_t fd = open("/dev/ttyMFD1", O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
  if (fd == -1) {
    printf("Failed to open port\r\n");
    return 1;
  } else {
    printf("Opened Port!\r\n");
  }

  if (tcgetattr(fd, &config) < 0) {
    printf("Unable to get config struct");
    close(fd);
    return 2;
  }
  
  setupSerialDevice(&config);
  if (tcsetattr(fd, TCSANOW, &config) < 0) {
    printf("Failed to set attributes");
  }
  return fd;
}
