#ifndef UIState_H_
#define UIState_H_
#include "Common.h"
#include "ScheduleState.h"
#include "I2CInterface.h"
#include "Forms.h"
#include <stack>
#include <vector>

#define MAX_PENDING_INPUT_EVENTS 10
#define MAX_SCREEN_HISTORY 32

class UIState
{
    public:
      UIState(ScheduleState *scheduleState, I2CInterface* i2cInterface) : display(i2cInterface), 
                                                                          inputEvents(MAX_SCREEN_HISTORY), 
                                                                          homeScreen(&display, scheduleState, this), 
                                                                          scheduleSelectForm(&display, scheduleState, this),
                                                                          messageForm(&display, scheduleState, this) { this->scheduleState = scheduleState; this->i2cInterface = i2cInterface; };
      void Setup();
      void Update();

      void ShowSelectScheduleForm() { PushScreen(scheduleSelectForm); } ;
      void ShowHomeForm() { PushScreen(homeScreen); } ;
      void ShowMessageForm(const char *message) { messageForm.SetMessage(message); PushScreen(messageForm); } ;
      void ShowPreviousScreen();

    private:
      int CurrentScreen;
      std::stack<BaseForm*> history;
      ScheduleState* scheduleState;
      I2CInterface* i2cInterface;
      Display display;

      HomeForm homeScreen;
      ScheduleSelectForm scheduleSelectForm;
      MessageForm messageForm;

      std::vector<InputEvent> inputEvents;

      void PushScreen(BaseForm& screenType);

      BaseForm* currentScreen() { return history.top(); };
};

#endif