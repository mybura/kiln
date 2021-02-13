#include "RelayControl.h"

void RelayControl::SetDutyCycle(float percentage)
{
  //Serial.print("Setting Duty Cycle: ");  Serial.println(percentage);
  OCR1B = 15624 * percentage * MAX_RELAY_ON_PERCENTAGE; 
}

void RelayControl::Setup()
{
  // Set to 1Hz PWM interval, giving control increment resolution of 1.6% for SSR over a 1s period.
  TCNT1  = 0;// Set Timer/Counter1 to 0
  OCR1B = 0; 
  pinMode(RELAY_PWM_PIN, OUTPUT);
  TCCR1A = _BV(COM1B1) | _BV(WGM11);                // Enable the PWM output OC1A on digital pins 9
  TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS12);     // Set fast PWM and prescaler of 256 on timer 1
  ICR1 = 15624;                                     // Set the PWM frequency to 1Hz: 16MHz/(256 * 4Hz) - 1 = 15624
}
