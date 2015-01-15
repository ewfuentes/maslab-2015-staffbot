#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <csignal>

#include "lidar.h"

#define MS 1000

struct termios config;

volatile uint8_t running = 1;
uint32_t samples[360] = {0xFFFFFFFF};

void setupSerialDevice(struct termios *cfg) {
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

void sigHandler(int signo) {
  if (signo == SIGINT) {
    printf("Keyboard Interrupt Caught\r\n");
    running = 0;
  }
}

void processLidarFrame(lidar_frame_t *frame) {
  uint16_t degreeStart = (frame->index - 0xA0) * 4;
  lidar_reading_t tmp;
  for (uint8_t i = 0; i < 4; i++) {
    tmp = frame->readings[i];
    uint32_t *bucket = &(samples[degreeStart + i]);
    if (tmp.invalidData) {
      *bucket = 0xFFFFFFFF;
    } else {
      *bucket = tmp.distance_mm;
    }
  }

  if (frame->index == 0xF9) {
    printf("*****START*****\r\n");
    for (uint16_t i = 0; i < 360; i++) {
        printf("%4d ", samples[i]);
        if (i % 20 == 19) {
          printf("\r\n");
        }
    }
    printf("*****END*****\r\n");
  }
}

int main(int argc, const char *argv[]) {
  signal(SIGINT, sigHandler);
  
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
  
  lidar_init((lidarFrameCallback_t)&processLidarFrame);

  printf("Device Configured\r\n");
  uint8_t readChar;
  while (running) {
    while (read(fd, &readChar, 1) > 0) {
      lidar_processByte(readChar);
    }
    usleep(10 * MS);
  }
  close(fd);
}
