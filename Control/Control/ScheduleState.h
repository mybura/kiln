#ifndef ScheduleState_H_
#define ScheduleState_H_

#include "Common.h"
#include "I2CInterface.h"
#include "SimpleTimer.h"

struct PhaseState
{
    int8_t Sequence;
    ValueLogger<double> Temperature;
    ValueLogger<double> RampRate;
    unsigned long int Time;
};

enum ScheduleType
{
  None,

  // Jewellery
  PureSilver,
  SterlingSilver,

  // Basic Functions
  BasicTemperature,
  BasicRamp,

  Test30
};

enum ScheduleStatus
{
    Started,
    Aborted,
    Paused,
    Completed
};

enum PhaseStatus
{
    WaitingTemperature,
    WaitingRampRate,
    WaitingTime
};

struct Schedule
{
    ScheduleType Type;
    char *Name;
    PhaseState *Phases[];
};

class ScheduleState
{
    public:
        ScheduleState(I2CInterface* i2cInterface) { this->i2cInterface = i2cInterface; Status = Aborted; scheduleType = None; };
        void Setup();
        void Update();

        void SetScheduleType(ScheduleType newType);
        void SetPhase(int phase);

        void Start();
        void Pause();
        void Resume();
        void Abort();

        void StartNextPhase();

        bool StateChanged() { return Changed || Current.Temperature.HasChanged() || Current.RampRate.HasChanged() || Target.RampRate.HasChanged() || Target.Temperature.HasChanged(); };
        void ClearChanged() { Changed = false; Current.Temperature.clearChanged(); Current.RampRate.clearChanged(); Target.RampRate.clearChanged(); Target.Temperature.clearChanged(); };

        float CurrentChanged;
        float TargetChanged;

        ValueLogger<ScheduleStatus> Status;
        ValueLogger<PhaseStatus> StatusPhase;
        ScheduleType Type() { return scheduleType; };

        PhaseState Current;
        PhaseState Target;
        PhaseState ExpectedTarget;

    private:
        ScheduleType scheduleType;
        SimpleTimer timer;
        int timerId;

        void StartTimer();

        void ClearState();
        void SafetyShutdown();

        bool Changed = true;
        I2CInterface* i2cInterface;
        float startingTemperature;

        unsigned long targetEndTime;
        unsigned long remainingTime;
};

#endif