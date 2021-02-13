#ifndef PIDInterface_H_
#define PIDInterface_H_

#include "ControllerState.h"
#include <NeoSWSerial.h>
#include <QueueArray.h>

#define RX_PIN A2
#define TX_PIN A3

#define LINE_BUFFER_SIZE 64         // Number of bytes to store as a line before discarding data
#define UART_SPEED 9600         

enum ECommandState
{
  WaitingCommandStart,
  WaitingCommand,
  WaitingParameter
};

union i2cfloat 
{
  float floatData;
  short rawData [sizeof (float)];
};

struct PIDCommand
{
  String Command;
  float Parameter;
  String StringParameter;
};

class PIDInterface
{ 
  public:
    PIDInterface(ControllerState* controllerState)  { this->controllerState = controllerState; };
    
    void Setup();
    void Update();
    
    void ReceiveEvent(int bytesReceived);
    void RequestEvent();

    void ProcessInput(QueueArray<String*> &dataLines);
    void ProcessCommands();

    float GetCurrentTemperature() { return controllerState->GetCurrentTemperature(); };
    float GetTargetTemperature() { return controllerState->GetTargetTemperature(); };
    float GetTargetRampRate() { return controllerState->GetTargetRampRate(); };
    void SetTargetTemperature(float temperature) { controllerState->SetTargetTemperature(temperature); };
    void SetTargetRampRate(float rampRate) { controllerState->SetTargetRampRate(rampRate); };


    NeoSWSerial* Serial;
  private:
    ECommandState currentCommandState;
    String lastCommand;
    String inputString;               // a String to hold incoming data
    QueueArray<PIDCommand*> commands;
    QueueArray<String*> dataLines;

    PIDInterface() {};

    ECommandState HandleCommand(String &command);
    ECommandState HandleParameter(String &parameter);
   
    ControllerState* controllerState;
    byte LastCommand;
};


#endif
