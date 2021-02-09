#ifndef Display_H_
#define Display_H_ 

#include "Common.h"
#include "Queue.h"

#define MAX_STATIC_STRINGS 20
#define MAX_STATIC_LENGTH 20
#define MAX_DYNAMIC_STRINGS 20
#define MAX_DYNAMIC_STRING_LENGTH 18
#define MAX_INSTRUCTIONS_PER_LOOP 1

#define LINE_SPACING_SMALL 5
#define LINE_SPACING_MEDIUM 8
#define LINE_SPACING_LARGE 12

#define BUTTON_SPACING_MEDIUM 10

// Instructions
//    Operation              Parameter0    Parameter1   Parameter2   Parameter3   Parameter4   
//    ---------              ----------    ----------   ----------   ----------   ----------   
//    FillScreen             color
//    FillRoundRect          x             y            w            h            Color
//    SetTextStyle           color         backgroundColor           textSize
//    PrintTextCenter        x             y            w            h            stringIndex
//    PrintText              x             y            stringIndex
//    PrintFloat             x             y            hi(value)    lo(value)    
//    FillRect               x             y            w            h            Color
//    SetText                index         length       variable_length(textValue) <- max MAX_DYNAMIC_STRING_LENGTH characters and less than 31 bytes

#define OP_FILLSCREEN       10
#define OP_FILLROUNDRECT    11
#define OP_SETTEXTSTYLE     12
#define OP_PRINTTEXTCENTER  13
#define OP_PRINTTEXT        14
#define OP_PRINTFLOAT       15
#define OP_FILLRECT         16
#define OP_SETTEXT          17

struct GPUInstruction
{
  byte Operation;
  union {
    uint16_t Parameter[5];
    byte ParameterB[10];
    float ParameterF[2];
  };
};

#define MAX_INSTRUCTIONS 64

class Display
{
  public:

    Display();
    void Setup();

    void Draw();
    
    void AddInstruction(GPUInstruction instruction) { instructions.push(instruction); };
    bool InstructionsOutstanding() { return instructions.count() > 0; } 
    
  private:
    volatile Queue<GPUInstruction> instructions;
    
    MCUFRIEND_kbv panel;
    
    void PrintText(uint16_t x, uint16_t y, uint8_t textSize, uint16_t color, uint16_t backgroundColor, const char *text);
    void PrintTextCenter(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t textSize, uint16_t color, uint16_t backgroundColor, const char *text);

    void PrintFloat(uint16_t x, uint16_t y, uint8_t textSize, uint16_t color, uint16_t backgroundColor, const char *format, double value, int precission);
    void PrintFloat(uint16_t x, uint16_t y, uint8_t textSize, uint16_t color, uint16_t backgroundColor, const char *format, double value, int precision, double defaultTriggerValue, char *defaultText);
    void PrintInterval(uint16_t x, uint16_t y, uint8_t textSize, uint16_t color, uint16_t backgroundColor, unsigned long int value);

    uint8_t TextSize;
    uint16_t TextColor;
    uint16_t TextBackgroundColor;
};

#endif

