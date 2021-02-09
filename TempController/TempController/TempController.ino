#include "Controllerstate.h"
#include "PIDInterface.h"
#include "I2CInterface.h"
#include "avr/wdt.h"

ControllerState controllerState;
PIDInterface pidInterface(controllerState);
I2CInterface i2cInterface(controllerState);

void setup() {
  Serial.begin(9600);
  Serial.println("Temperature Controller V1 .... Operational!");

  pidInterface.Setup();
  i2cInterface.Setup();

  wdt_enable(WDTO_8S);
}

void loop() 
{
  controllerState.Update();
  pidInterface.Update();
  i2cInterface.Update();

  wdt_reset();
}
 