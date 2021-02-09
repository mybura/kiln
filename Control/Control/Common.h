#ifndef Common_H_
#define Common_H_

#include "Arduino.h"

#ifndef min
//#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#define SCHEDULE_STATE_MIN 0

template <class T>
class ValueLogger
{
  T value;
  bool changed;

public:

  T& operator=(const T& other)
  {
    changed = (value != other);
    value = other;
    return value;
  }

  operator T() const
  {
    return value;
  }

  bool HasChanged() { return changed; };
  void clearChanged() { changed = false; }
};

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