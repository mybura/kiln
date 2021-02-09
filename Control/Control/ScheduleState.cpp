#include "ScheduleState.h"
#include <PolledTimeout.h>

extern ScheduleState scheduleState;

void TimeElapsed()
{
  scheduleState.StartNextPhase();
}  

void ScheduleState::Setup() 
{ 
  StatusPhase = WaitingTemperature;
  Status = Aborted;
  CurrentChanged = false;
  TargetChanged = false;

  timerId = -1;
  Changed = true; 
  ClearState();
}

#define HOURS_TO_MILLIS(x) (x * 60l * 60l * 1000l)
#define MINUTES_TO_MILLIS(x) (x * 60l * 1000l)
#define SECONDS_TO_MILLIS(x) (x * 1000l)

// **************
#define SCHEDULE_START(scheduleType) case scheduleType : \
                                     {                   \
                                        switch (phase)   \
                                        {
                                          
#define PHASE(sequence,temperature,ramprate,time) case sequence : result.Sequence = sequence; result.Temperature = temperature; result.RampRate = ramprate; result.Time = time; break;
#define DEFAULT_PHASE default : result.Sequence = -1; result.Temperature = SCHEDULE_STATE_MIN; result.RampRate = SCHEDULE_STATE_MIN; result.Time = SCHEDULE_STATE_MIN;
#define SCHEDULE_END                    DEFAULT_PHASE \
                                        }; break; \
                                      };
// **************

PhaseState GetSchedulePhase(ScheduleType scheduleType, int phase)
{
  PhaseState result;

  switch (scheduleType)
  {
    SCHEDULE_START(None)
    PHASE(1,    SCHEDULE_STATE_MIN, SCHEDULE_STATE_MIN, SCHEDULE_STATE_MIN)
    SCHEDULE_END

    // PMC Clay
    SCHEDULE_START(PureSilver)
    PHASE(1,    900,    0,  HOURS_TO_MILLIS(4))
    SCHEDULE_END

    // PMC OneFire Sterling Clay
    SCHEDULE_START(SterlingSilver)
    PHASE(1,    900,    0,  HOURS_TO_MILLIS(1))
    SCHEDULE_END

    SCHEDULE_START(Test30)
    PHASE(1,    30,    0,  SECONDS_TO_MILLIS(10))
    PHASE(2,    33,    0,  SECONDS_TO_MILLIS(5))
    SCHEDULE_END

    DEFAULT_PHASE
  }
    
  return result;
}


void ScheduleState::SetPhase(int phase)
{
  ExpectedTarget = GetSchedulePhase(scheduleType, phase);
  remainingTime = ExpectedTarget.Time;
  Target = ExpectedTarget;
  ClearState();
}

void ScheduleState::SetScheduleType(ScheduleType newType)
{
  ClearState();
  scheduleType = newType;
  SetPhase(1);
}

void ScheduleState::ClearState()
{
  Current.Sequence = 1;
  Current.Temperature = SCHEDULE_STATE_MIN;
  Current.RampRate = SCHEDULE_STATE_MIN;
  Current.Time = SCHEDULE_STATE_MIN;
  StatusPhase = WaitingTemperature;
}


void ScheduleState::StartTimer()
{
  if (ExpectedTarget.Time != SCHEDULE_STATE_MIN)
  {
    targetEndTime = millis() + ExpectedTarget.Time;
    timerId = this->timer.setTimeout(ExpectedTarget.Time, TimeElapsed);
  }
}

void ScheduleState::Start()
{
  targetEndTime = millis() + ExpectedTarget.Time;
  Status = Started;

  i2cInterface->SetTargetTemperature(ExpectedTarget.Temperature);
  i2cInterface->SetTargetRampRate(ExpectedTarget.RampRate);

  startingTemperature = i2cInterface->GetCurrentTemperature();
}

void ScheduleState::Pause()
{
  remainingTime = targetEndTime - millis();

  Status = Paused;
  if (timerId > -1) { this->timer.deleteTimer(timerId); timerId = -1; }

  i2cInterface->SetTargetTemperature(Current.Temperature);
  i2cInterface->SetTargetRampRate(0);
}

void ScheduleState::Resume()
{
  targetEndTime = millis() + remainingTime;
  Status = Started;

  i2cInterface->SetTargetTemperature(ExpectedTarget.Temperature);
  i2cInterface->SetTargetRampRate(ExpectedTarget.RampRate);
}

void ScheduleState::Abort()
{
  remainingTime = 0;
  Status = Aborted;

  if (timerId > -1) { this->timer.deleteTimer(timerId); timerId = -1; }
  SafetyShutdown();
  SetPhase(1);    // Reset to the start of this schedule
}

void ScheduleState::StartNextPhase()
{
  if (timerId > -1) { this->timer.deleteTimer(timerId); timerId = -1; }
  
  auto nextPhase = GetSchedulePhase(scheduleType, ExpectedTarget.Sequence + 1).Sequence;
  if (nextPhase < 0)
  {
      SafetyShutdown();
      Status = Completed;
      SetPhase(1);    // Reset to the start of this schedule
  }
  else
  {
    SetPhase(nextPhase);
    Start();
  }
}

void ScheduleState::SafetyShutdown()
{
  i2cInterface->SetTargetTemperature(0);
  i2cInterface->SetTargetRampRate(0);
}

void ScheduleState::Update()
{
    this->timer.run();

    using periodic = esp8266::polledTimeout::periodic;
    static periodic updateInterval(300);

    if (updateInterval) 
    {       
        Current.Temperature = i2cInterface->GetCurrentTemperature();
        Current.RampRate = i2cInterface->GetCurrentRampRate();
        
        if (Status == Started || Status == Paused)
        {
          Target.Temperature = i2cInterface->GetTargetTemperature();
          Target.RampRate = i2cInterface->GetTargetRampRate();
        }
        else
        {
          Target.Temperature = ExpectedTarget.Temperature;
          Target.RampRate = ExpectedTarget.RampRate;
        }
    }

    auto currentTime = millis();

    if (currentTime > targetEndTime)
        remainingTime = 0;
    else
        remainingTime = targetEndTime - currentTime;

  auto timerEnabled = (timerId > -1);

  if (!timerEnabled && 
      Current.Temperature > ExpectedTarget.Temperature && 
      Status == Started) 
  {
    StatusPhase = WaitingTime;
    StartTimer();
  }
  else if (timerEnabled && 
           ExpectedTarget.Time > SCHEDULE_STATE_MIN)
  {
    auto before = Current.Time;
    
    Current.Time = ExpectedTarget.Time - remainingTime;

    if (Current.Time != 0)
      Current.RampRate = (Current.Temperature - startingTemperature) / Current.Time;
    else
      Current.RampRate = 0;
  }
  else if (!timerEnabled && Current.Temperature < ExpectedTarget.Temperature && Status == Started)
    StatusPhase = WaitingTemperature;
  else if (timerEnabled && remainingTime <= SCHEDULE_STATE_MIN)
    SafetyShutdown();

}
