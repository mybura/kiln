#include "Forms.h"
#include "UIState.h"

bool HomeForm::Update()
{
  auto shouldDraw = true;

  StartButton.Visible = ((scheduleState->Status == Aborted) || (scheduleState->Status == Completed)) && (scheduleState->Type() != None); 
  AbortButton.Visible = (scheduleState->Status == Started) && (scheduleState->Type() != None); 
  PauseButton.Visible = (scheduleState->Status == Started) && (scheduleState->Type() != None); 
  ResumeButton.Visible = (scheduleState->Status == Paused) && (scheduleState->Type() != None); 
  NewButton.Visible = (scheduleState->Status == Aborted) || (scheduleState->Status == Completed); 

  if (scheduleState->Status == Completed && scheduleState->Status.HasChanged())
  {
    uiState->ShowMessageForm("Schedule Completed");
    scheduleState->Status.clearChanged();
    shouldDraw = false;
  }

  return shouldDraw;
}

void HomeForm::HandleStartPressed()
{
  scheduleState->Start();
}

void HomeForm::HandleNewPressed()
{  
  uiState->ShowSelectScheduleForm();
}

HomeForm::HomeForm(Display *display, ScheduleState *scheduleState, UIState *uiState) : BaseForm(display, scheduleState, uiState) 
{ 
  NewButton.Action = [this]() -> void { HandleNewPressed(); };
  NewButton.Title = "new";
  NewButton.Bounds = GRID_BIG(1,0);
  NewButton.FaceColor = COLOR_BUTTON_FACE_ACTION;
  NewButton.TitleColor = COLOR_BUTTON_TEXT;
  NewButton.TitleSize = TEXT_MEDIUM;
  Buttons.push_back(&NewButton);

  StartButton.Action = [this]() -> void { HandleStartPressed(); };
  StartButton.Title = "start";
  StartButton.Bounds = GRID_BIG(0,0);
  StartButton.FaceColor = COLOR_BUTTON_FACE_ACTION;
  StartButton.TitleColor = COLOR_BUTTON_TEXT;
  StartButton.TitleSize = TEXT_MEDIUM;
  Buttons.push_back(&StartButton);

  AbortButton.Action = [this]() -> void { HandleAbortPressed(); };
  AbortButton.Title = "abort";
  AbortButton.Bounds = GRID_BIG(1,0);
  AbortButton.FaceColor = COLOR_BUTTON_FACE_NEGATIVE;
  AbortButton.TitleColor = COLOR_BUTTON_TEXT;
  AbortButton.TitleSize = TEXT_MEDIUM;
  Buttons.push_back(&AbortButton);

  PauseButton.Action = [this]() -> void { HandlePausePressed(); };
  PauseButton.Title = "pause";
  PauseButton.Bounds = GRID_BIG(0,0);
  PauseButton.FaceColor = COLOR_BUTTON_FACE_ACTION;
  PauseButton.TitleColor = COLOR_BUTTON_TEXT;
  PauseButton.TitleSize = TEXT_MEDIUM;
  Buttons.push_back(&PauseButton);

  ResumeButton.Action = [this]() -> void { HandleResumePressed(); };
  ResumeButton.Title = "resume";
  ResumeButton.Bounds = GRID_BIG(0,0);
  ResumeButton.FaceColor = COLOR_BUTTON_FACE_ACTION;
  ResumeButton.TitleColor = COLOR_BUTTON_TEXT;
  ResumeButton.TitleSize = TEXT_MEDIUM;
  Buttons.push_back(&ResumeButton);
}

void HomeForm::HandleAbortPressed()
{
  scheduleState->Abort();
}

void HomeForm::HandlePausePressed()
{
  scheduleState->Pause();
}

void HomeForm::HandleResumePressed()
{
  scheduleState->Resume();
}

void BaseForm::DrawButton(Button &b)
{
  display->FillRoundRect(b.Bounds.TopLeft.x, 
                         b.Bounds.TopLeft.y, 
                         b.Bounds.BottomRight.x - b.Bounds.TopLeft.x, 
                         b.Bounds.BottomRight.y - b.Bounds.TopLeft.y, 
                         b.FaceColor);
                      
  display->PrintTextCenter(b.Bounds.TopLeft.x , 
                           b.Bounds.TopLeft.y, 
                           b.Bounds.BottomRight.x - b.Bounds.TopLeft.x,
                           b.Bounds.BottomRight.y - b.Bounds.TopLeft.y, 
                           b.TitleSize, 
                           b.TitleColor, 
                           b.FaceColor,
                           b.Title);                     
}

void BaseForm::ClearButton(Button &b)
{
  display->FillRoundRect(b.Bounds.TopLeft.x, 
                         b.Bounds.TopLeft.y, 
                         b.Bounds.BottomRight.x - b.Bounds.TopLeft.x, 
                         b.Bounds.BottomRight.y - b.Bounds.TopLeft.y, 
                         COLOR_BACKGROUND_DARK);
}

#define DRAW_IF_BUTTON_VISIBLE(button) if (ScreenChanged || button.Visible.HasChanged()) if (button.Visible) DrawButton(button);
#define FIELD_OR_SCREEN_CHANGED(field) if (field.HasChanged() || ScreenChanged) 
#define width DISPLAY_WIDTH
#define height DISPLAY_HEIGHT
#define halfWidth (width / 2)
#define halfHeight (height / 2)
#define quarterHeight (halfHeight / 2)
#define thirdHeight (height / 3)

void HomeForm::Draw()
{
    if (Update())
    {
      auto currentY = 5;
      auto currentX = 5;
      auto currentChanged = scheduleState->CurrentChanged;
      auto targetChanged = scheduleState->TargetChanged;

      if (ScreenChanged || scheduleState->StateChanged())
      {
        if (ScreenChanged)
        {
          // Draw backgrounds
          display->FillRect(0, 0, width, thirdHeight, COLOR_BACKGROUND_DARK);
          display->FillRect(0, thirdHeight, width, thirdHeight * 2, COLOR_BACKGROUND_LIGHT);
        }

        if (!NewButton.Visible && !AbortButton.Visible)
        {
          ClearButton(AbortButton);
        }

        // Buttons 
        DRAW_IF_BUTTON_VISIBLE(NewButton)
        DRAW_IF_BUTTON_VISIBLE(StartButton)
        DRAW_IF_BUTTON_VISIBLE(AbortButton)
        DRAW_IF_BUTTON_VISIBLE(ResumeButton)
        DRAW_IF_BUTTON_VISIBLE(PauseButton)
          
        // Subsystem Status
        currentY = thirdHeight + LINE_SPACING_LARGE;
        currentX = 195;

        if (ScreenChanged)
        {
          display->PrintText(10, currentY, TEXT_MEDIUM, COLOR_LABEL_ON_LIGHT, COLOR_LABEL_ON_LIGHT, "Current");
          display->PrintText(195, currentY, TEXT_MEDIUM, COLOR_LABEL_ON_LIGHT, COLOR_LABEL_ON_LIGHT, "Target");
        }

        currentX = 10;
        currentY = halfHeight + LINE_SPACING_SMALL;

        auto valueColor = scheduleState->StatusPhase == WaitingTemperature ? COLOR_IN_PROGRESS : COLOR_LABEL_ON_LIGHT;
        FIELD_OR_SCREEN_CHANGED(scheduleState->Current.Temperature)
          display->PrintFloat(currentX, currentY, TEXT_XLARGE, valueColor, COLOR_BACKGROUND_LIGHT, "%7s", scheduleState->Current.Temperature, 1, SCHEDULE_STATE_MIN, " ----.-");
        
        currentY += LINE_SPACING_LARGE * 4;

        valueColor = scheduleState->StatusPhase == WaitingRampRate ? COLOR_IN_PROGRESS : COLOR_LABEL_ON_LIGHT;
        FIELD_OR_SCREEN_CHANGED(scheduleState->Current.RampRate)
        {
          auto rampRateInUOM = scheduleState->Current.RampRate * 60.0 * 60.0 * 1000;   // RampRate per hour.
          if (rampRateInUOM > 9999) rampRateInUOM = 9999.99;
          display->PrintFloat(currentX + 80, currentY, TEXT_MEDIUM, valueColor, COLOR_BACKGROUND_LIGHT, "%7s", rampRateInUOM, 2, SCHEDULE_STATE_MIN, "----.--");
        }

        currentY += LINE_SPACING_LARGE * 3;
        valueColor = scheduleState->StatusPhase == WaitingTime ? COLOR_IN_PROGRESS : COLOR_LABEL_ON_LIGHT;
        display->PrintInterval(currentX + 70, currentY, TEXT_MEDIUM, valueColor, COLOR_BACKGROUND_LIGHT, scheduleState->Current.Time);

        currentX = 195;
        currentY = halfHeight;
        currentY += LINE_SPACING_SMALL;
        if (ScreenChanged) display->PrintText(currentX, currentY, TEXT_SMALL, COLOR_NORMAL_ON_LIGHT, COLOR_BACKGROUND_LIGHT, "Temperature");
    
        currentY += LINE_SPACING_LARGE;
        FIELD_OR_SCREEN_CHANGED(scheduleState->Target.Temperature)
          display->PrintFloat(currentX, currentY, TEXT_MEDIUM, COLOR_NORMAL_ON_LIGHT, COLOR_BACKGROUND_LIGHT, "%6s\xA7" "C", scheduleState->Target.Temperature, 0, SCHEDULE_STATE_MIN, "  ----\xA7" "C");
    
        currentY += LINE_SPACING_LARGE * 2;
        if (ScreenChanged) display->PrintText(currentX, currentY, TEXT_SMALL, COLOR_NORMAL_ON_LIGHT, COLOR_BACKGROUND_LIGHT, "Ramp Rate");
        
        currentY += LINE_SPACING_LARGE;
        FIELD_OR_SCREEN_CHANGED(scheduleState->Target.RampRate)
        {
          auto rampRateInUOM = scheduleState->Target.RampRate * 60 * 60 * 1000;   // RampRate per hour.
          display->PrintFloat(currentX, currentY, TEXT_MEDIUM, COLOR_NORMAL_ON_LIGHT, COLOR_BACKGROUND_LIGHT, "%6s\xA7" "C/h", rampRateInUOM, 0, SCHEDULE_STATE_MIN, " ---.-\xA7" "C/h");
        }
    
        currentY += LINE_SPACING_LARGE * 2;
        if (ScreenChanged) display->PrintText(currentX, currentY, TEXT_SMALL, COLOR_NORMAL_ON_LIGHT, COLOR_BACKGROUND_LIGHT, "Hold Time (h:m:s)");
        
        currentY += LINE_SPACING_LARGE;
        display->PrintInterval(currentX, currentY, TEXT_MEDIUM, COLOR_NORMAL_ON_LIGHT, COLOR_BACKGROUND_LIGHT, scheduleState->Target.Time);

        scheduleState->ClearChanged();     
      }

      ScreenChanged = false;;
    }
}

void ScheduleSelectForm::Draw()
{
    Update();

    if (ScreenChanged)
    {
      // Draw background
      if (ScreenChanged)
      {
        display->FillRect(0, 0, width, height, COLOR_BACKGROUND_DARK);
      }

      DRAW_IF_BUTTON_VISIBLE(BackButton)
      DRAW_IF_BUTTON_VISIBLE(PureSilverButton)
      DRAW_IF_BUTTON_VISIBLE(SSilverButton)
      DRAW_IF_BUTTON_VISIBLE(CustomButton)
    }

    ScreenChanged = false;;
}

ScheduleSelectForm::ScheduleSelectForm(Display *display, ScheduleState *scheduleState, UIState *uiState) : BaseForm(display, scheduleState, uiState) 
{ 
  BackButton.Action = [this]() -> void { BaseForm::uiState->ShowPreviousScreen(); };
  BackButton.Title = "back";
  BackButton.Bounds = GRID_BIG(1,0);
  BackButton.FaceColor = COLOR_BUTTON_FACE_ACTION;
  BackButton.TitleColor = COLOR_BUTTON_TEXT;
  BackButton.TitleSize = TEXT_MEDIUM;
  Buttons.push_back(&BackButton);  

  PureSilverButton.Action = [this]() -> void { BaseForm::scheduleState->SetScheduleType(PureSilver); BaseForm::uiState->ShowPreviousScreen(); };
  PureSilverButton.Title = "999 silver";
  PureSilverButton.Bounds = GRID_BIG(0,1);
  PureSilverButton.FaceColor = COLOR_BUTTON_FACE_ACTION;
  PureSilverButton.TitleColor = COLOR_BUTTON_TEXT;
  PureSilverButton.TitleSize = TEXT_MEDIUM;
  Buttons.push_back(&PureSilverButton);  

  SSilverButton.Action = [this]() -> void { BaseForm::scheduleState->SetScheduleType(SterlingSilver); BaseForm::uiState->ShowPreviousScreen(); };
  SSilverButton.Title = "925 silver";
  SSilverButton.Bounds = GRID_BIG(1,1);
  SSilverButton.FaceColor = COLOR_BUTTON_FACE_ACTION;
  SSilverButton.TitleColor = COLOR_BUTTON_TEXT;
  SSilverButton.TitleSize = TEXT_MEDIUM;
  Buttons.push_back(&SSilverButton);  

  CustomButton.Action = [this]() -> void { BaseForm::scheduleState->SetScheduleType(Test30); BaseForm::uiState->ShowPreviousScreen(); };
  CustomButton.Title = "custom";
  CustomButton.Bounds = GRID_BIG(0,2);
  CustomButton.FaceColor = COLOR_BUTTON_FACE_ACTION;
  CustomButton.TitleColor = COLOR_BUTTON_TEXT;
  CustomButton.TitleSize = TEXT_MEDIUM;
  Buttons.push_back(&CustomButton);  
}

void ScheduleSelectForm::Update()
{
  BackButton.Visible = true; 
  CustomButton.Visible = true; 
  SSilverButton.Visible = true; 
  PureSilverButton.Visible = true; 
}

void MessageForm::Draw()
{
    Update();

    if (ScreenChanged)
    {
      // Draw background and message
      if (ScreenChanged)
      {
        display->FillRect(0, 0, width, height, COLOR_BACKGROUND_LIGHT);
        display->PrintTextCenter(0, 0, width, thirdHeight * 2, TEXT_MEDIUM, COLOR_NORMAL_ON_LIGHT, COLOR_BACKGROUND_LIGHT, message);
      }

      DRAW_IF_BUTTON_VISIBLE(OkayButton)
    }

    ScreenChanged = false;;
}

MessageForm::MessageForm(Display *display, ScheduleState *scheduleState, UIState *uiState) : BaseForm(display, scheduleState, uiState) 
{ 
  OkayButton.Action = [this]() -> void { BaseForm::uiState->ShowPreviousScreen(); };
  OkayButton.Title = "ok";
  OkayButton.Bounds = GRID_BIG(0.5,2);
  OkayButton.FaceColor = COLOR_BUTTON_FACE_ACTION;
  OkayButton.TitleColor = COLOR_BUTTON_TEXT;
  OkayButton.TitleSize = TEXT_MEDIUM;
  Buttons.push_back(&OkayButton);  
}

void MessageForm::Update()
{
  OkayButton.Visible = true; 
}
