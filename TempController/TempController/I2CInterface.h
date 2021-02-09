#ifndef I2CInterface_H_
#define I2CInterface_H_

#include "ControllerState.h"

#define I2C_BUS_ID 56

#define SDA_PIN 4
#define SCL_PIN 5

// Max time to wait for data before a packet is considered malformed
#define PACKET_TIMEOUT 10

// Direct Temperature Control
#define COMMAND_SET_TARGET_TEMP 10
#define COMMAND_SET_TARGET_RAMP 11
#define COMMAND_GET_TARGET_TEMP 12
#define COMMAND_GET_TARGET_RAMP 13

#define COMMAND_GET_CURRENT_TEMP 15
#define COMMAND_GET_CURRENT_RAMP 16

#define COMMAND_DISABLE_WATCHDOG 18

class I2CInterface
{
  public:

    I2CInterface(ControllerState& controllerState) { this->controllerState = controllerState; };
    void Setup();
    void Update();

    void OnDataReceived(int bytesReceived);
    void OnDataRequested();
  private:
  
    uint16_t ReadRegisterIndex = COMMAND_GET_TARGET_TEMP;
    ControllerState& controllerState;
};

#endif

