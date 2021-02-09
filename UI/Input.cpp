#include "Input.h"
#include "Common.h"

#include "Display.h"
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>

// Constructor 
Input::Input() 
{
  isTouching = false;
  targetRelease = MAXPRESSURE;
}

void Input::Setup()
{
  
}
void Input::ProcessInput()
{
  static bool alreadyProcessing = false;

  // Avoid interrupt re-entrancy
  if (!alreadyProcessing)
  {
    alreadyProcessing = true;
    RefreshInputState();
    alreadyProcessing = false;
  }
}

void Input::RefreshInputState()
{
  TSPoint tp = TouchPanel.getPoint();

  // Sharing pins with the TFT module, so fix the directions of the touchscreen pins
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  bool readingValid = (tp.z != 0);

  if (readingValid)
  {
    bool newIsTouching = (tp.z > MINPRESSURE && tp.z < targetRelease);
    
    if (isTouching != newIsTouching)
    {
      isTouching = newIsTouching;      

      if (isTouching)
      {
        targetRelease = tp.z;
        AddEvent(tp, Touched);
      }
      else
      {
        targetRelease = MAXPRESSURE;
        AddEvent(tp, Released);
      }
    }    
  }
}

// TODO figure out why NewPressed does not get assigned when setting the zone config. Only new pressed does not work, other work fine.

void Input::AddEvent(TSPoint &point, EEventInputType eventType)
{
  // Transform touch panel coordinates to display coordinates. Because the display is landscape, swap the x/y values.
  auto x = map(point.y, TS_TOP, TS_BOT, 0, DISPLAY_WIDTH);
  auto y = map(point.x, TS_RT, TS_LEFT, 0, DISPLAY_HEIGHT);

  InputEvent event;
  
  event.EventType = eventType;
  event.y = y;
  event.x = x;
  events.lockedPush(event); 
}

