#include "Motor.h"

Motor::Motor(uint8_t pwmPin, uint8_t dirPin) {
  pwmp = new mraa::Pwm(pwmPin);
  if (pwmp == NULL) {
    printf("PWMP is NULL\r\n");
  }

  pwmp->write(0);
  pwmp->enable(true);

  dirp = new mraa::Gpio(dirPin);
  dirp->dir(mraa::DIR_OUT); 
}

void Motor::setSpeed(float pwm,bool direction) {
  pwmp->write(pwm);
  dirp->write(direction);
}
