#include "Controllerstate.h"
#include "PIDInterface.h"
#include "I2CInterface.h"
#include "avr/wdt.h"

ControllerState* controllerState;
PIDInterface* pidInterface;
I2CInterface* i2cInterface;
bool tuningStarted = false;

void setup() {
  Serial.begin(9600);
  Serial.println("Temperature Controller V1 .... Operational!");
 
  controllerState = new ControllerState();
  pidInterface = new PIDInterface(controllerState);
  i2cInterface = new I2CInterface(controllerState);

  pidInterface->Setup();
  i2cInterface->Setup();

  wdt_enable(WDTO_8S);
}

void loop() 
{
  controllerState->Update();
  pidInterface->Update();
  i2cInterface->Update();

  wdt_reset();
  if (!tuningStarted)
  {
    tuningStarted = true;
  //  controllerState->StartTuning();
  } 
}
 
