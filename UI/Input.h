#ifndef Input_H_
#define Input_H_

#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>
#include "Common.h"
#include "Style.h"
#include <RingBuf.h>

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

// Calibration and touch panel configuration
 //240x320 ID=0x6767
#define XP 7
#define XM LCD_WR
#define YP LCD_CD
#define YM 6
#define TS_LEFT 229
#define TS_RT 805
#define TS_TOP 168
#define TS_BOT 899

// TODO Put these in config to adjust for user's stylus. Fingers seem to work between 50 and 250 where sharp objects seem to be 50 to 500
#define MINPRESSURE 50       // Hard presses go below this value or its an incorrect reading
#define MAXPRESSURE 250      // Lifting the stylus will cause values above this but the screen has a natural rebound round about this value when stylus is completely removed

#define MAX_EVENTS  10  // Max number of queued input events

class Input
{
  public:

    Input();
    void Setup();
    void ProcessInput();

    bool HasInputEvents() { return !events.isEmpty(); };
    InputEvent GetNextInputEvent() { InputEvent result; events.pop(result); return result; };
    byte PendingEventsCount() { return events.size(); };
    
  private:

    int16_t targetRelease;
    bool isTouching;
    
    TouchScreen TouchPanel = TouchScreen(XP, YP, XM, YM, MAXPRESSURE);
    RingBuf<InputEvent, MAX_EVENTS> events;

    void RefreshInputState();
    void AddEvent(TSPoint &point, EEventInputType eventType);   
};

#endif

