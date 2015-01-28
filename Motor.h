#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h>
#include "mraa.hpp"

class Motor {
    mraa::Pwm *pwmp;
    mraa::Gpio *dirp;
    
  public:
    //inputs are pin numbers
    Motor(uint8_t pwmPin, uint8_t dirPin);

    // pwm value must be between 0 and 1
    // direction will go forward if >= 0, backward if < 0
    void setSpeed(float pwm, bool direction);
};

#endif
