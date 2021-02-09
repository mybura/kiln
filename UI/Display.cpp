#include "Display.h"
#include "Common.h"
#include "Style.h"

#include <Adafruit_GFX.h>    // Core graphics library
#include <MCUFRIEND_kbv.h>

const char string_0[MAX_STATIC_LENGTH] PROGMEM = "start";
const char string_1[MAX_STATIC_LENGTH] PROGMEM = "new";
const char string_2[MAX_STATIC_LENGTH] PROGMEM = "resume";
const char string_3[MAX_STATIC_LENGTH] PROGMEM = "stop";
const char string_4[MAX_STATIC_LENGTH] PROGMEM = "Current";
const char string_5[MAX_STATIC_LENGTH] PROGMEM = "Target";
const char string_6[MAX_STATIC_LENGTH] PROGMEM = "Temperature";
const char string_7[MAX_STATIC_LENGTH] PROGMEM = "Ramp Rate";
const char string_8[MAX_STATIC_LENGTH] PROGMEM = "Hold Time (h:m:s)";
const char string_9[MAX_STATIC_LENGTH] PROGMEM = "abort";
const char string_10[MAX_STATIC_LENGTH] PROGMEM = "pause";

const char* const ConstantStrings[MAX_STATIC_STRINGS] PROGMEM = {
  string_0, 
  string_1,
  string_2,
  string_3,
  string_4,
  string_5,
  string_6,
  string_7,
  string_8,
  string_9,
  string_10
};
char DynamicStrings[MAX_DYNAMIC_STRINGS][MAX_DYNAMIC_STRING_LENGTH];

// Constructor for Display
Display::Display() : instructions(MAX_INSTRUCTIONS)
{
  TextSize = TEXT_MEDIUM;
  TextColor = COLOR_LABEL_ON_LIGHT;
  TextBackgroundColor = COLOR_LABEL_ON_LIGHT;
}

// Update display with expected visual state
void Display::Draw() 
{
  byte instructionsProcessed = 0;

  GPUInstruction instruction;
  while (instructions.count() > 0 && 
         (instructionsProcessed < MAX_INSTRUCTIONS_PER_LOOP))
  {
    noInterrupts();
    instruction = instructions.pop();
    interrupts();

    switch (instruction.Operation)
    {
      case (OP_FILLSCREEN): panel.fillScreen(instruction.Parameter[0]); break; 
      case (OP_FILLRECT): panel.fillRect(instruction.Parameter[0], instruction.Parameter[1], instruction.Parameter[2], instruction.Parameter[3], instruction.Parameter[4]); break;
      case (OP_FILLROUNDRECT): panel.fillRoundRect(instruction.Parameter[0], instruction.Parameter[1], instruction.Parameter[2], instruction.Parameter[3], BUTTON_EDGE_RADIUS, instruction.Parameter[4]); break;
      case (OP_PRINTTEXT): 
      {
        auto stringIndex = instruction.Parameter[2];

        // Index > 128 = constant strings
        if (stringIndex >= 128)
        {
          char buffer[MAX_STATIC_LENGTH + 1];
          strncpy_PF(buffer, (char *)pgm_read_word(&(ConstantStrings[stringIndex - 128])), MAX_STATIC_LENGTH);
          buffer[MAX_STATIC_LENGTH] = 0;
          
          PrintText(instruction.Parameter[0], instruction.Parameter[1], TextSize, TextColor, TextBackgroundColor, buffer);
        }
        else
        {
          auto value = DynamicStrings[stringIndex];
          PrintText(instruction.Parameter[0], instruction.Parameter[1], TextSize, TextColor, TextBackgroundColor, value);
        }
        
        break;
      }
      case (OP_PRINTTEXTCENTER): 
      {
        auto stringIndex = instruction.Parameter[4];

        // Index > 128 = constant strings
        if (stringIndex >= 128)
        {
          char buffer[MAX_STATIC_LENGTH + 1];
          strncpy_PF(buffer, (char *)pgm_read_word(&(ConstantStrings[stringIndex - 128])), MAX_STATIC_LENGTH);
          buffer[MAX_STATIC_LENGTH] = 0;
          
          PrintTextCenter(instruction.Parameter[0], instruction.Parameter[1], instruction.Parameter[2], instruction.Parameter[3], TextSize, TextColor, TextBackgroundColor, buffer);
        }
        else
        {
          auto value = DynamicStrings[stringIndex];
          PrintTextCenter(instruction.Parameter[0], instruction.Parameter[1], instruction.Parameter[2], instruction.Parameter[3], TextSize, TextColor, TextBackgroundColor, value);
        }
        
        break;
      }
      case (OP_PRINTFLOAT): 
      {
          PrintFloat(instruction.Parameter[0], instruction.Parameter[1], TextSize, TextColor, TextBackgroundColor, "%7s", instruction.ParameterF[1], 2);
      }      
      case (OP_SETTEXTSTYLE):
      { 
        TextSize = instruction.Parameter[2];
        TextColor = instruction.Parameter[0];
        TextBackgroundColor = instruction.Parameter[1];
        break;
      }      
    }  
    
    instructionsProcessed++; 
  }
}

void Display::Setup()
{
  pinMode(LCD_CD, OUTPUT);
  pinMode(LCD_WR, OUTPUT);

  panel.reset();                 // Hardware reset
  uint16_t ID = panel.readID();  

  panel.begin(ID);
  panel.setRotation(1);          // Landscape
}

char *ftoa(char *a, double f, int precision)
{
 long p[] = {0,10,100,1000,10000,100000,1000000,10000000,100000000};
 
 char *ret = a;
 long heiltal = (long)f;
 itoa(heiltal, a, 10);
 while (*a != '\0') a++;
 if (precision > 0)
 {
   *a++ = '.';
   long desimal = abs((long)((f - heiltal) * p[precision]));
   itoa(desimal, a, 10);
 }
 return ret;
}

void Display::PrintTextCenter(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t textSize, uint16_t color, uint16_t backgroundColor, const char *text)
{
    uint16_t targetX, targetY, textWidth, textHeight;

    panel.setTextSize(textSize);
    panel.getTextBounds(text, x, y, &targetX, &targetY, &textWidth, &textHeight);
  
    targetX = x + (x - targetX) + (width / 2) - (textWidth / 2);
    targetY = y + (y - targetY) + (height / 2) - (textHeight / 2);

    PrintText(targetX, targetY, textSize, color, backgroundColor, text);
}

void Display::PrintText(uint16_t x, uint16_t y, uint8_t textSize, uint16_t color, uint16_t backgroundColor, const char *text)
{
    panel.setCursor(x, y);
    panel.setTextSize(textSize);
    if (backgroundColor == color)
      panel.setTextColor(color);
    else
      panel.setTextColor(color, backgroundColor);
    panel.print(text);
}

void Display::PrintFloat(uint16_t x, uint16_t y, uint8_t textSize, uint16_t color, uint16_t backgroundColor, const char *format, double value, int precision, double defaultTriggerValue, char *defaultText)
{
  if (value == defaultTriggerValue)
    PrintText(x, y, textSize, color, backgroundColor, defaultText);
  else
    PrintFloat(x, y, textSize, color, backgroundColor, format, value, precision);
}

void Display::PrintFloat(uint16_t x, uint16_t y, uint8_t textSize, uint16_t color, uint16_t backgroundColor, const char *format, double value, int precision)
{
    char buffer1[20];
    char buffer2[20];

    ftoa(buffer1, value, precision);
    sprintf(buffer2, format, buffer1);

    PrintText(x, y, textSize, color, backgroundColor, buffer2);
}

void Display::PrintInterval(uint16_t x, uint16_t y, uint8_t textSize, uint16_t color, uint16_t backgroundColor, unsigned long int value)
{
  auto valueSeconds = value / 1000;
  char buffer1[10];
  sprintf(buffer1, "%02lu:%02lu:%02lu", (valueSeconds / 60 / 60) % 60, (valueSeconds / 60) % 60, valueSeconds % 60);

  if (value == 0)
    PrintText(x, y, textSize, color, backgroundColor, "--:--:--");
  else
    PrintText(x, y, textSize, color, backgroundColor, buffer1);
}


