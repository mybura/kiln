#ifndef Input_H_
#define Input_H_

#include "Common.h"

enum EEventInputType
{
  Ignore = 0,
  
  Touched = 1,
  Released = 2,
  SlideUp = 3,
  SlideDown = 4,
  SlideLeft = 5,
  SlideRight = 6
};

struct InputEvent
{
  byte EventType;
  uint16_t x, y;
};

#endif

