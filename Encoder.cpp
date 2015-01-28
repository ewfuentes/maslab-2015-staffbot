#include "Encoder.h"
#include <cassert>
#include <iostream>


struct encoderISRArgs {
  Encoder *obj;
  mraa::Gpio *pin;
};

int getPhase(uint8_t a, uint8_t b) {
  assert(a < 2); // Can only be 0 or 1
  assert(b < 2); // Can only be 0 or 1

  return ((a & 0x01) << 1) | (b & 0x01);
}

void edgeISRWrapper(void *args) {
  struct encoderISRArgs *isrArgs = (encoderISRArgs *)args;
  Encoder *obj = isrArgs->obj;
  mraa::Gpio *pin = isrArgs->pin;
  obj->edgeISR(pin);
}

void Encoder::edgeISR(mraa::Gpio *pin) {
  assert(pin == phaseAp || pin == phaseBp);

  uint8_t a = aState;
  uint8_t b = bState;

  uint8_t newState = pin->read();

  uint8_t prevPhase = getPhase(a,b);

  uint8_t newPhase = 0xFF;
  if (pin == phaseAp) {
    aState = newState;
    newPhase = getPhase(newState, b); 
  } else if (pin == phaseBp){
    bState = newState;
    newPhase = getPhase(a, newState);
  }

  updateTick(prevPhase, newPhase);
}


void Encoder::updateTick(uint8_t oldPhase, uint8_t newPhase) {
  int8_t delta = newPhase - oldPhase;
  if ((delta == 1) || (delta == -3)) {
    count++;
  } else if ((delta  == -1) || (delta == 3)) {
    count--;
  } else {
    std::cerr << "Weird phase change: " << oldPhase 
              << " to " << newPhase << std::endl;
  }
}


Encoder::Encoder(uint8_t phaseApin, uint8_t phaseBpin) {
  phaseAp = new mraa::Gpio(phaseApin);
  phaseBp = new mraa::Gpio(phaseBpin);
 
  isrArgsA = new encoderISRArgs;
  isrArgsA->obj = this;
  isrArgsA->pin = phaseAp;

  isrArgsB = new encoderISRArgs;
  isrArgsB->obj = this;
  isrArgsB->pin = phaseBp;

  phaseAp->dir(mraa::DIR_IN);
  phaseBp->dir(mraa::DIR_IN);

  phaseAp->isr(mraa::EDGE_BOTH, edgeISRWrapper, isrArgsA);
  phaseBp->isr(mraa::EDGE_BOTH, edgeISRWrapper, isrArgsB);
}


