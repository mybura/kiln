#include "I2CInterface.h"
#include <Wire.h>

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

extern char DynamicStrings[MAX_DYNAMIC_STRINGS][MAX_DYNAMIC_STRING_LENGTH];

void I2CInterface::OnDataReceived(int bytesReceived)
{ 
  while (Wire.available() > 0)
  {
    char command = Wire.read();
    GPUInstruction instruction;

    instruction.Operation = command;

    switch (command)
    {        
        // 1 parameter operations
        case OP_FILLSCREEN: { 
            Wire.readBytes((char *)&instruction.Parameter[0], 2); 
            break;
        }
        // 3 parameter operations
        case OP_SETTEXTSTYLE:
        case OP_PRINTTEXT: { 
            Wire.readBytes((char *)&instruction.Parameter[0], 2); 
            Wire.readBytes((char *)&instruction.Parameter[1], 2); 
            Wire.readBytes((char *)&instruction.Parameter[2], 2); 
            break;   
        }       
        // 4 parameter operations
        case OP_PRINTFLOAT: { 
            Wire.readBytes((char *)&instruction.Parameter[0], 2); 
            Wire.readBytes((char *)&instruction.Parameter[1], 2); 
            Wire.readBytes((char *)&instruction.Parameter[2], 2); 
            Wire.readBytes((char *)&instruction.Parameter[3], 2); 
            break;
        }
        // 5 parameter operations
        case OP_FILLROUNDRECT: 
        case OP_PRINTTEXTCENTER:
        case OP_FILLRECT: {
            Wire.readBytes((char *)&instruction.Parameter[0], 2); 
            Wire.readBytes((char *)&instruction.Parameter[1], 2); 
            Wire.readBytes((char *)&instruction.Parameter[2], 2); 
            Wire.readBytes((char *)&instruction.Parameter[3], 2); 
            Wire.readBytes((char *)&instruction.Parameter[4], 2); 
            break;            
        }
        case OP_SETTEXT:
        {
            Wire.readBytes((char *)&instruction.Parameter[0], 2); 
            Wire.readBytes((char *)&instruction.Parameter[1], 2); 
            
            if (instruction.Parameter[0] >= MAX_DYNAMIC_STRINGS)
                instruction.Parameter[0] = MAX_DYNAMIC_STRINGS - 1;
            
            auto stringLength = instruction.Parameter[1];
            if (stringLength >= MAX_DYNAMIC_STRING_LENGTH)
                stringLength = MAX_DYNAMIC_STRING_LENGTH;

            Wire.readBytes((char *)&DynamicStrings[instruction.Parameter[0]], instruction.Parameter[1]); 
            DynamicStrings[instruction.Parameter[0]][instruction.Parameter[1]] = 0; // Slap a terminating 0 to the end of the string
            break;
        }
    }
    
    display->AddInstruction(instruction);
  }
}

void I2CInterface::OnDataRequested()
{
    if (input->HasInputEvents())
    {
        auto event = input->GetNextInputEvent();

        // Send number of events and one event's detail
        Wire.write(input->PendingEventsCount()); 
        Wire.write((char *)&event, 5);
    }
    else 
    {
        // Feedback no input data
        InputEvent event;
        event.EventType = Ignore;
        event.x = 0;
        event.y = 0;  

        Wire.write(input->PendingEventsCount()); 
        Wire.write((char *)&event, 5); 
    }
}

