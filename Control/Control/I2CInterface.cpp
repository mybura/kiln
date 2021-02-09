#include "Wire.h"
#include "I2CInterface.h"
#include "Display.h"
#include "PID.h"

#define SDA_PIN 4
#define SCL_PIN 5

void I2CInterface::Setup()
{
    Wire.begin(SDA_PIN, SCL_PIN);
}

void SendInstruction(GPUInstruction& instruction, byte numberOfParameterBytes)
{
    Wire.beginTransmission(I2C_UI_BUS_ID);
    Wire.write(instruction.Operation);
    Wire.write((byte *)&instruction.Parameter[0], numberOfParameterBytes);
    Wire.setClockStretchLimit(1500);    // Needed because the NANO controller is 2 slow for the ESP
    Wire.endTransmission();
}

void I2CInterface::FillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    GPUInstruction instruction;
    instruction.Operation = OP_FILLRECT;
    instruction.Parameter[0] = x;
    instruction.Parameter[1] = y;
    instruction.Parameter[2] = w;
    instruction.Parameter[3] = h;
    instruction.Parameter[4] = color;

    SendInstruction(instruction, 10);
}

void I2CInterface::FillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    GPUInstruction instruction;
    instruction.Operation = OP_FILLROUNDRECT;
    instruction.Parameter[0] = x;
    instruction.Parameter[1] = y;
    instruction.Parameter[2] = w;
    instruction.Parameter[3] = h;
    instruction.Parameter[4] = color;

    SendInstruction(instruction, 10);
}

void I2CInterface::PrintText(int16_t x, int16_t y, byte index)
{
    GPUInstruction instruction;
    instruction.Operation = OP_PRINTTEXT;
    instruction.Parameter[0] = x;
    instruction.Parameter[1] = y;
    instruction.Parameter[2] = index;

    SendInstruction(instruction, 6);
}

void I2CInterface::PrintTextCenter(int16_t x, int16_t y, int16_t w, int16_t h, byte index)
{
    GPUInstruction instruction;
    instruction.Operation = OP_PRINTTEXTCENTER;
    instruction.Parameter[0] = x;
    instruction.Parameter[1] = y;
    instruction.Parameter[2] = w;
    instruction.Parameter[3] = h;
    instruction.Parameter[4] = index;

    SendInstruction(instruction, 10);
}

void I2CInterface::PrintFloat(int16_t x, int16_t y, float value)
{
    GPUInstruction instruction;
    instruction.Operation = OP_PRINTFLOAT;
    instruction.Parameter[0] = x;
    instruction.Parameter[1] = y;
    instruction.ParameterF[1] = value;

    SendInstruction(instruction, 8);
}

void I2CInterface::SetTextStyle(uint16_t color, uint16_t backgroundColor, uint16_t textSize)
{
    GPUInstruction instruction;
    instruction.Operation = OP_SETTEXTSTYLE;
    instruction.Parameter[0] = color;
    instruction.Parameter[1] = backgroundColor;
    instruction.Parameter[2] = textSize;
    
    SendInstruction(instruction, 8);
}

void I2CInterface::SetTextValue(uint16_t index, const char* value)
{
    GPUInstruction instruction;
    instruction.Operation = OP_SETTEXT;
    instruction.Parameter[0] = index;
    instruction.Parameter[1] = strlen(value);
    
    Wire.beginTransmission(I2C_UI_BUS_ID);
    Wire.write(instruction.Operation);
    Wire.write((byte *)&instruction.Parameter[0], 4);
    Wire.write((byte *)value, instruction.Parameter[1]);
    Wire.endTransmission();
}

float GetPIDRegisterValue(byte command)
{
    float result = 0;

    Wire.beginTransmission(I2C_PID_BUS_ID);
    Wire.write(command);
    if (Wire.endTransmission() == 0)
    {
        Wire.requestFrom(I2C_PID_BUS_ID, 4);
        Wire.readBytes((byte *)&result, 4);
    }

    return result;   
}

InputEvent I2CInterface::GetInputEvent()
{    
    InputEvent result;

    Wire.requestFrom(I2C_UI_BUS_ID, 6);
    RemainingInputQueueSize = Wire.read();
    result.EventType = Wire.read();
    Wire.readBytes((byte *)&result.x, 4);

    return result;   
}

float I2CInterface::GetCurrentTemperature()
{
    return GetPIDRegisterValue(COMMAND_GET_CURRENT_TEMP);
}

float I2CInterface::GetCurrentRampRate()
{
    return GetPIDRegisterValue(COMMAND_GET_CURRENT_RAMP);
}

float I2CInterface::GetTargetTemperature()
{
    return GetPIDRegisterValue(COMMAND_GET_TARGET_TEMP);
}

float I2CInterface::GetTargetRampRate()
{
    return GetPIDRegisterValue(COMMAND_GET_TARGET_RAMP);
}

void I2CInterface::SetTargetTemperature(float value)
{
    Wire.beginTransmission(I2C_PID_BUS_ID);
    Wire.write(COMMAND_SET_TARGET_TEMP);
    Wire.write((byte *)&value, sizeof(value));
    Wire.endTransmission();
}

void I2CInterface::SetTargetRampRate(float value)
{
    Wire.beginTransmission(I2C_PID_BUS_ID);
    Wire.write(COMMAND_SET_TARGET_RAMP);
    Wire.write((byte *)&value, sizeof(value));
    Wire.endTransmission();
}

void I2CInterface::DisablePIDWatchDog()
{
    Wire.beginTransmission(I2C_PID_BUS_ID);
    Wire.write(COMMAND_DISABLE_WATCHDOG);
    Wire.endTransmission();
}