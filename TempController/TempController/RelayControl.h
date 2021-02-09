#ifndef _RELAY_CONTROL_H
#define _RELAY_CONTROL_H

#include "Arduino.h"

// Limits output duty cycle for testing. Final version will be 1.0 to allow the PWM to go all the way to 100%
#define MAX_RELAY_ON_PERCENTAGE 1.0

// WARNING: If you change this to another pin, the PWM needs to be configured on different registers. Pin 10 uses the OCR1B register.
#define RELAY_PWM_PIN 10

class RelayControl {
 public:
    void Setup();
 
    void SetDutyCycle(float percentage);
};

#endif
