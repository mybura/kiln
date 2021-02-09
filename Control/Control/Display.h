#ifndef Display_H_
#define Display_H_ 

#include "Common.h"
#include "I2CInterface.h"

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

class Display 
{
  public:
      Display(I2CInterface* i2cInterface) { this->i2cInterface = i2cInterface; };

      void PrintFloat(uint16_t x, uint16_t y, uint8_t textSize, uint16_t color, uint16_t backgroundColor, const char *format, double value, int precision, double defaultTriggerValue, char *defaultText);
      void PrintFloat(uint16_t x, uint16_t y, uint8_t textSize, uint16_t color, uint16_t backgroundColor, const char *format, double value, int precision);
      void PrintInterval(uint16_t x, uint16_t y, uint8_t textSize, uint16_t color, uint16_t backgroundColor, unsigned long int value);
      void PrintText(uint16_t x, uint16_t y, uint8_t textSize, uint16_t color, uint16_t backgroundColor, const char *text);
      void PrintTextCenter(uint16_t x, uint16_t y, int16_t w, int16_t h, uint8_t textSize, uint16_t color, uint16_t backgroundColor, const char *text);
      void FillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) { i2cInterface->FillRect(x, y, w, h, color); };
      void FillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) { i2cInterface->FillRoundRect(x, y, w, h, color); };

  private:
      I2CInterface* i2cInterface;
};

#endif

