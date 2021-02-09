#include "I2CInterface.h"
#include <Wire.h>
#include "avr/wdt.h"


volatile I2CInterface* I2CInterfaceHandler;

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {
    I2CInterfaceHandler->OnDataReceived(howMany);
}

void requestEvent() {
    I2CInterfaceHandler->OnDataRequested();
}
 
void I2CInterface::Setup()
{
    I2CInterfaceHandler = this;
    Wire.begin(I2C_BUS_ID);

    // deactivate internal pullups for twi.
    digitalWrite(SDA, 0);
    digitalWrite(SCL, 0);

    Wire.onReceive(receiveEvent); // register event
    Wire.onRequest(requestEvent); // register event
    Wire.setTimeout(PACKET_TIMEOUT);
}

void I2CInterface::Update()
{

}

void I2CInterface::OnDataReceived(int bytesReceived)
{ 
  while (Wire.available() > 0)
  {
    int command = Wire.read();

    switch (command)
    {        
        case COMMAND_GET_TARGET_TEMP: 
        case COMMAND_GET_TARGET_RAMP: 
        case COMMAND_GET_CURRENT_TEMP: 
        case COMMAND_GET_CURRENT_RAMP:         
        {
            ReadRegisterIndex = command;
            break;
        }
        case COMMAND_DISABLE_WATCHDOG:
        {
            wdt_disable();
            break;
        }
        case COMMAND_SET_TARGET_TEMP:
        case COMMAND_SET_TARGET_RAMP:
        { 
            float parameter = 0;
            Wire.readBytes((char *)&parameter, sizeof(parameter)); 

            if (command == COMMAND_SET_TARGET_TEMP)
                controllerState.SetTargetTemperature(parameter);
            else if (command == COMMAND_SET_TARGET_RAMP)
                controllerState.SetTargetRampRate(parameter);

            break;   
        }       
    }
  }
}

void I2CInterface::OnDataRequested()
{
    float returnValue = 0;
    switch (ReadRegisterIndex)
    {
        case COMMAND_GET_TARGET_TEMP: returnValue = controllerState.GetTargetTemperature(); break;
        case COMMAND_GET_TARGET_RAMP: returnValue = controllerState.GetTargetRampRate(); break;
        case COMMAND_GET_CURRENT_TEMP: returnValue = controllerState.GetCurrentTemperature(); break; 
        case COMMAND_GET_CURRENT_RAMP: returnValue = controllerState.GetCurrentRampRate(); break;
    }

    Wire.write((byte *)&returnValue, sizeof(returnValue));
}

