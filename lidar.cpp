#include "lidar.h"

#include <stdio.h>

#define START 0xFA
#define FRAME_LENGTH 22

typedef enum {
  lidarState_lookingForStart,
  lidarState_receivingFrames
} lidarState_t;

uint8_t frameBuf[22] = {0};
uint8_t frameIdx = 0; 

static lidarState_t state = lidarState_lookingForStart;

static uint16_t computeChecksum(uint16_t *buf) {
  uint32_t checksum = 0;
  uint8_t i = 0;
  for (i = 0; i < FRAME_LENGTH/2 - 1; i++) {
    checksum = (checksum << 1) + buf[i];
  }
  checksum = (checksum & 0x7FFF) + (checksum >> 15);
  checksum = checksum & 0x7FFF;
  return checksum;
}

static lidarState_t lookingForStart(uint8_t b) {
  if (b == START && frameIdx == 0) {
    frameBuf[frameIdx++] = b;
  } else if (frameIdx > 0) {
    frameBuf[frameIdx++] = b;
  }

  if (frameIdx == FRAME_LENGTH) {
    uint16_t checksum = computeChecksum((uint16_t *)frameBuf);
    uint16_t rxChecksum = (frameBuf[20] & 0xFF) | ((frameBuf[21] & 0xFF) << 8);
    
    printf("checksum: %0.4x\r\n", checksum);
    printf("rx checksum: %0.4x\r\n", rxChecksum);
    
    frameIdx = 0;
    if (checksum == rxChecksum) {
      return lidarState_receivingFrames;
      printf("Found Start!!");
    }
  }
  return lidarState_lookingForStart;
}

static void processFrame(uint8_t *buf) {
  lidar_frame_t *frame = (lidar_frame_t *)buf;
  lidar_reading_t *temp;
  if (frame->index == 0xA1){
    for (uint8_t i = 0; i < 1; i++) {
      temp = &(frame->readings[i]);
      printf("Distance: %5d ", temp->distance_mm);
      printf("Strength Warning: %1d ", temp->strengthWarning);
      printf("Invalid Data: %1d ", temp->invalidData);
      printf("Strength: %5d ", temp->strength);
    }
    printf("\r\n");
  }
//  printf("Received Frame: ");
//  for (uint8_t i = 0; i < FRAME_LENGTH; i++) {
//    printf("%0.2X ", buf[i]);
//  } 
//  printf("\r\n");
}

static lidarState_t receiveFrames(uint8_t b) {
  frameBuf[frameIdx++] = b;
  if (frameIdx == FRAME_LENGTH) {
    uint16_t checksum = computeChecksum((uint16_t *)frameBuf);
    uint16_t rxChecksum = (frameBuf[FRAME_LENGTH - 2] & 0xFF) | 
                          ((frameBuf[FRAME_LENGTH - 1] & 0xFF) << 8);
    frameIdx = 0;
    if (checksum != rxChecksum) {
      return lidarState_lookingForStart;
    }
    processFrame(frameBuf);
  }
  return lidarState_receivingFrames;
}

void lidar_processByte(uint8_t b) {
  switch (state) {
    case lidarState_lookingForStart:
      state = lookingForStart(b);
      break;
    case lidarState_receivingFrames:
      state = receiveFrames(b);     
      break;
  }
}
