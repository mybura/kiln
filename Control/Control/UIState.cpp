#include "UIState.h"
#include "Style.h"
#include <PolledTimeout.h>

void UIState::Setup() 
{ 
    PushScreen(homeScreen);
};
    
void UIState::Update () 
{ 
    using periodic = esp8266::polledTimeout::periodic;
    using oneShot = esp8266::polledTimeout::oneShot;
    static periodic updateInterval(500);
    static periodic updateInputInterval(50);
    static oneShot debounceInterval(500);

    if (updateInputInterval) 
    {       
      auto inputEvent = i2cInterface->GetInputEvent();
      do 
      {
        if (inputEvent.EventType != Ignore) inputEvents.push_back(inputEvent);
        
        inputEvent = i2cInterface->GetInputEvent();
      } while (i2cInterface->RemainingInputQueueSize > 0);
    }

    if (updateInterval) 
    {       
      if (currentScreen() != NULL) currentScreen()->Draw();
    }

    while (inputEvents.size() > 0)
    {
        auto event = inputEvents.back();
        inputEvents.pop_back();

        auto form = currentScreen();
        if (form != nullptr && debounceInterval)
        {
            for (int i = 0; i < form->Buttons.size(); i++)
            {
                auto button = form->Buttons[i];

                if (button->Visible &&
                    event.EventType == Released &&
                    event.x >= button->Bounds.TopLeft.x && 
                    event.x <= button->Bounds.BottomRight.x && 
                    event.y >= button->Bounds.TopLeft.y && 
                    event.y <= button->Bounds.BottomRight.y)
                {
                    debounceInterval.reset();
                    button->Action();
                }            
            }
        }
    }
};

void UIState::PushScreen(BaseForm& form)
{
    history.push(&form);
    currentScreen()->Show();
    currentScreen()->Draw();
}

void UIState::ShowPreviousScreen()
{
    history.pop();
    currentScreen()->Show();
    currentScreen()->Draw();
}
