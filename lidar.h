#ifndef LIDAR_H
#define LIDAR_H

#include <stdint.h>

typedef struct {
  uint16_t distance_mm:14;
  uint8_t strengthWarning:1;
  uint8_t invalidData:1;
  uint16_t strength:16;
} lidar_reading_t;

typedef struct {
  uint8_t startByte;
  uint8_t index;
  uint16_t rpm_10q6;
  lidar_reading_t readings[4];
  uint16_t checksum;
} lidar_frame_t;

typedef void (*lidarFrameCallback_t)(lidar_frame_t *);

void lidar_init(lidarFrameCallback_t cb);
void lidar_processByte(uint8_t byte);

#endif

