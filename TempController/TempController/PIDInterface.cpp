#include "PIDInterface.h"
#include "avr/wdt.h"

#define COMMAND_START "~1"

// Direct Temperature Control
#define COMMAND_SET_TARGET_TEMP "STT"
#define COMMAND_SET_TARGET_RAMP "STR"
#define COMMAND_GET_TARGET_TEMP "GTT"
#define COMMAND_GET_TARGET_RAMP "GTR"

#define COMMAND_GET_CURRENT_TEMP "GCT"

#define COMMAND_DISABLE_WATCHDOG "DWD"

// Reply message commands
const String REPLY_GET_TARGET_TEMP = "RTT";
const String REPLY_GET_CURRENT_TEMP = "RCT";
const String REPLY_GET_TARGET_RAMP = "RTR";
const String REPLY_GET_CURRENT_RAMP = "RCR";

PIDInterface::PIDInterface(ControllerState& controllerState) : Serial((uint8_t) RX_PIN, (uint8_t) TX_PIN)
{ 
  this->controllerState = controllerState; 

  pinMode(RX_PIN, INPUT);
  pinMode(TX_PIN, OUTPUT);
};

void PIDInterface::Setup()
{
  inputString.reserve(LINE_BUFFER_SIZE);
  lastCommand.reserve(LINE_BUFFER_SIZE);
  inputString = "";
  
  currentCommandState = WaitingCommandStart;
  Serial.begin(UART_SPEED);
}

void PIDInterface::ProcessInput(QueueArray<String*> &dataLines)
{
  while (!dataLines.isEmpty())
  {
    auto currentLine = dataLines.dequeue();
    
    switch (currentCommandState)
    {
      case WaitingCommandStart: if (*currentLine == COMMAND_START) currentCommandState = WaitingCommand; break;
      case WaitingCommand: currentCommandState = HandleCommand(*currentLine); break;
      case WaitingParameter: currentCommandState = HandleParameter(*currentLine); break;
      default: ::Serial.print("Unknown command state") ; ::Serial.println(currentCommandState); 
    }
    
    delete currentLine;
  }
}

ECommandState PIDInterface::HandleCommand(String &command)
{
  auto result = WaitingCommandStart;
  PIDCommand* newCommand = NULL;

  newCommand = new PIDCommand;
  lastCommand = command;
  
  if (command == COMMAND_GET_TARGET_TEMP)
    { newCommand->Command = command; }
  else if (command == COMMAND_GET_CURRENT_TEMP)
    { newCommand->Command = command; }
  else if (command == COMMAND_GET_TARGET_RAMP)
    { newCommand->Command = command; }
  else if (command == COMMAND_DISABLE_WATCHDOG)
    { newCommand->Command = command; }    
  else if (command == COMMAND_SET_TARGET_TEMP)
    { result = WaitingParameter; delete newCommand; newCommand = NULL; }
  else if (command == COMMAND_SET_TARGET_RAMP)
    { result = WaitingParameter; delete newCommand; newCommand = NULL; }
  else { delete newCommand; newCommand = NULL; Serial.print("Unknown command: '"); Serial.print(command); Serial.println("'");}

  if (newCommand != NULL)
    commands.enqueue(newCommand);

  return result;
}


ECommandState PIDInterface::HandleParameter(String &parameter)
{
  auto result = WaitingCommandStart;
  PIDCommand* newCommand = NULL;

  newCommand = new PIDCommand;
  if (lastCommand == COMMAND_SET_TARGET_TEMP)
    { newCommand->Command = lastCommand; }
  else if (lastCommand == COMMAND_SET_TARGET_RAMP)
    { newCommand->Command = lastCommand; }
  else { delete newCommand; newCommand = NULL; }

  if (newCommand != NULL)
  {
    if ((lastCommand == COMMAND_SET_TARGET_RAMP) || 
        (lastCommand == COMMAND_SET_TARGET_TEMP))
      newCommand->Parameter = parameter.toFloat();
    else
      newCommand->StringParameter = parameter;
      
    commands.enqueue(newCommand);
  }

  return result;
}

void PIDInterface::Update()
{
  while (Serial.available()) {
    auto inChar = (char)Serial.read();

    if (inChar == '\n' || inChar == '\r') {
      if ((char)Serial.peek() == '\n') Serial.read();  //  Handle CRLF
      dataLines.enqueue(new String(inputString));
      inputString = "";
    }
    else if (inputString.length() < LINE_BUFFER_SIZE)
      inputString += inChar;
  }

  ProcessInput(dataLines);
  ProcessCommands();
}

#define MAX_COMMANDS_PROCESS_PER_LOOP 2

void PIDInterface::ProcessCommands()
{
  auto processedCount = 0;
  while (!commands.isEmpty() && processedCount < MAX_COMMANDS_PROCESS_PER_LOOP)
  {
    processedCount++;
    auto currentCommand = commands.dequeue();
    auto reply = String(COMMAND_START) + " ";

    if (currentCommand->Command == COMMAND_GET_CURRENT_TEMP)
      reply = reply + REPLY_GET_CURRENT_TEMP + " " + GetCurrentTemperature() + " C";
    else if (currentCommand->Command == COMMAND_GET_TARGET_TEMP)
      reply = reply + REPLY_GET_TARGET_TEMP + " " + GetTargetTemperature() + " C";
    else if (currentCommand->Command == COMMAND_GET_TARGET_RAMP)
      reply = reply + REPLY_GET_TARGET_RAMP + " " + GetTargetRampRate() + " C/S";
    else if (currentCommand->Command == COMMAND_SET_TARGET_TEMP)
    {
      SetTargetTemperature(currentCommand->Parameter); 
      reply = reply + REPLY_GET_TARGET_TEMP + " " + GetTargetTemperature() + " C";
    }
    else if (currentCommand->Command == COMMAND_SET_TARGET_RAMP)
    {
      SetTargetRampRate(currentCommand->Parameter);
      reply = reply + REPLY_GET_TARGET_RAMP + " " + GetTargetRampRate() + " C/S";
    }
    else if (currentCommand->Command == COMMAND_DISABLE_WATCHDOG)
    {
      wdt_disable();
      reply = reply + " WATCHDOG DISABLED";
    }    
    else
      reply += " UNKNOWN COMMAND";

    Serial.println(reply);
    
    delete currentCommand;
  }
}