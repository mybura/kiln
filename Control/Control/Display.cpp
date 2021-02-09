#include "Common.h"
#include "Display.h"
#include "stdio.h"

byte nextStringIndex = 0;

void IncreaseNextStringIndex()
{
    nextStringIndex = (nextStringIndex + 1);   // Use the next string index because if the GPU is slow, previous pending PrintText and SetTextValue combinations will use the latest labels incorrectly.
    if (nextStringIndex >= MAX_STRING_INDEX)
      nextStringIndex = 0;
}

void Display::PrintText(uint16_t x, uint16_t y, uint8_t textSize, uint16_t color, uint16_t backgroundColor, const char *text)
{
    i2cInterface->SetTextValue(nextStringIndex, text);
    i2cInterface->SetTextStyle(color, backgroundColor, textSize);
    i2cInterface->PrintText(x, y, nextStringIndex);
    IncreaseNextStringIndex();
}

void Display::PrintTextCenter(uint16_t x, uint16_t y, int16_t w, int16_t h, uint8_t textSize, uint16_t color, uint16_t backgroundColor, const char *text)
{
    i2cInterface->SetTextValue(nextStringIndex, text);
    i2cInterface->SetTextStyle(color, backgroundColor, textSize);
    i2cInterface->PrintTextCenter(x, y, w, h, nextStringIndex);
    IncreaseNextStringIndex();
}

void Display::PrintFloat(uint16_t x, uint16_t y, uint8_t textSize, uint16_t color, uint16_t backgroundColor, const char *format, double value, int precision, double defaultTriggerValue, char *defaultText)
{
  if (value == defaultTriggerValue)
    PrintText(x, y, textSize, color, backgroundColor, defaultText);
  else
    PrintFloat(x, y, textSize, color, backgroundColor, format, value, precision);
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

void Display::PrintFloat(uint16_t x, uint16_t y, uint8_t textSize, uint16_t color, uint16_t backgroundColor, const char *format, double value, int precision)
{
    char buffer1[50];
    char buffer2[50];

    ftoa(buffer1, value, precision);
    sprintf(buffer2, format, buffer1);
    buffer2[9] = 0;

    PrintText(x, y, textSize, color, backgroundColor, buffer2);
}

void Display::PrintInterval(uint16_t x, uint16_t y, uint8_t textSize, uint16_t color, uint16_t backgroundColor, unsigned long int value)
{
  auto valueSeconds = value / 1000;
  char buffer1[10];
  sprintf(buffer1, "%02lu:%02lu:%02lu", (valueSeconds / 60 / 60) % 60, (valueSeconds / 60) % 60, valueSeconds % 60);
  buffer1[9] = 0;

  if (value == 0)
    PrintText(x, y, textSize, color, backgroundColor, "--:--:--");
  else
    PrintText(x, y, textSize, color, backgroundColor, buffer1);
}
