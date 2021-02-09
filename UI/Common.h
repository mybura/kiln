#ifndef Common_H_
#define Common_H_

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#include "Adafruit_GFX.h" // Hardware-specific library
#include <MCUFRIEND_kbv.h>
#include <MemoryFree.h> 

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

// Assign human-readable names to some common 16-bit color values:
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define PANEL_WIDTH 320
#define PANEL_HEIGHT 240

struct Point
{
  uint16_t x;
  uint16_t y;
};

struct Rect
{
  Point TopLeft;
  Point BottomRight;
};

#define RECT_INTERSECT(rect1,rect2) ((rect2.TopLeft.x >= rect1.TopLeft.x && rect2.TopLeft.x <= rect1.BottomRight.x) && \
                                     (rect2.TopLeft.y >= rect1.TopLeft.y && rect2.TopLeft.y <= rect1.BottomRight.y))

#define RECT_CONTAINS(rect1,pointX,pointY) ((pointX >= rect1.TopLeft.x && pointX <= rect1.BottomRight.x) && \
                                            (pointY >= rect1.TopLeft.y && pointY <= rect1.BottomRight.y))

#define CONST_RECT(left,top,right,bottom) {{left,top}, {right,bottom}}

#endif

