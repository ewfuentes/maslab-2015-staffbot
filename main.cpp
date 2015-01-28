#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <csignal>

#include "uart.h"
#include "lidar.h"

#define MS 1000

volatile uint8_t running = 1;
uint32_t samples[360] = {0xFFFFFFFF};

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
  int32_t fd = uart_init();
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
