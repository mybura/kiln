#include "Common.h"
#include "UIState.h"
#include "ScheduleState.h"
#include "I2CInterface.h"
#include <ESP8266WiFi.h>

I2CInterface i2cInterface;
ScheduleState scheduleState(&i2cInterface);
UIState uiState(&scheduleState, &i2cInterface);

void setup() {
  Serial.begin(115200);
  WiFi.forceSleepBegin();

  // Wait for other controllers to start up
  delay(3000);

  i2cInterface.Setup();
  uiState.Setup();
  scheduleState.Setup();
}

void loop() {
  i2cInterface.Update();
  scheduleState.Update();
  uiState.Update();
}

//:\Users\Scanner\AppData\Local\Arduino15\packages\esp8266\hardware\esp8266\2.5.0/tools/sdk/include
