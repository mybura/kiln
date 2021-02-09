#ifndef Forms_H_
#define Forms_H_ 

#include "ScheduleState.h"
#include "Display.h"
#include "Style.h"
#include "Button.h"
#include <vector>

class UIState;

class BaseForm
{
    public:
        BaseForm(Display *display, ScheduleState *scheduleState, UIState *uiState) { this->display = display; this->scheduleState = scheduleState; this->uiState = uiState; }

        virtual void Show() { ScreenChanged = true; };
        virtual void Draw() { return; };

        std::vector<Button *> Buttons;

        Display* display;
        ScheduleState* scheduleState;
        UIState* uiState;

    protected:
        bool ScreenChanged;

        void DrawButton(Button &b);
        void ClearButton(Button &b);
};

class HomeForm : public BaseForm
{
    public:
        HomeForm(Display *display, ScheduleState *scheduleState, UIState *uiState);

        bool Update();
        void Draw() override;

        void HandleStartPressed();
        void HandleNewPressed();
        void HandleAbortPressed();
        void HandlePausePressed();
        void HandleResumePressed();

    private:
        Button StartButton;
        Button NewButton;
        Button AbortButton;
        Button ResumeButton;
        Button PauseButton;
};

class ScheduleSelectForm : public BaseForm
{
    public:
        ScheduleSelectForm(Display *display, ScheduleState *scheduleState, UIState *uiState);

        void Update();
        void Draw() override;

    private:
        Button BackButton;
        Button CustomButton;
        Button PureSilverButton;
        Button SSilverButton;
};

class MessageForm : public BaseForm
{
    public:
        MessageForm(Display *display, ScheduleState *scheduleState, UIState *uiState);

        void SetMessage(const char *message) { this->message = message; }
        void Update();
        void Draw() override;

    private:
        Button OkayButton;
        const char *message;
};

#endif