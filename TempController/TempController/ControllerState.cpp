#include "ControllerState.h"

#define RAMP_FILTER_MA_SAMPLES 20
#define RAMPRATE_SAMPLING_INTERVAL 5000

ControllerState::ControllerState() : 
  thermoAmp(MAXCLK, MAXCS, MAXDO), 
  pid(&CurrentTemperature, &PIDOutput, &TargetTemperature, KP, KI, KD, DIRECT),
  pidATune(&CurrentTemperature, &PIDOutput),
  rampRate(RAMP_FILTER_MA_SAMPLES),
  relayControl()
{
    #ifdef SIMULATE_TEMPRETATURE_IN
  CurrentTemperature = 10;
    #else
  CurrentTemperature = 0;
    #endif
  TargetTemperature = 0;
  CurrentRampRate = 0;
  TargetRampRate = 0;
  lastTemperatureRead = 0;
  LastUpdateTime = millis();
  
  pid.SetOutputLimits(0, 255); // 255 = 100% duty cycle on PWM
  pid.SetMode(AUTOMATIC);

  rampRate.begin();
  relayControl.Setup();
  
  // wait for MAX chip to stabilize
  delay(500);
}

void ControllerState::StartTuning()
{
  tuning = false;
  ChangeAutoTune();
  tuning = true;
}

void ControllerState::Update()
{
  // TODO Watchdog on Temp feedback and current used
  float c = thermoAmp.readFilteredCelsius();
  if (isnan(c)) 
  {
     Serial.println("Something wrong with thermocouple!");
  } 
  else 
  {
    #ifdef SIMULATE_TEMPRETATURE_IN
      if (PIDOutput == 0)
      {
        CurrentTemperature -= 0.5;
      }
      else
        CurrentTemperature += (PIDOutput / 255.0) * 0.1;
    #else
      CurrentTemperature = c;
    #endif
  };

  if(tuning)
  {
    byte val = (pidATune.Runtime());
    
    if (val!=0)
    {
      tuning = false;
    }
    if(!tuning)
    { 
      //we're done, set the tuning parameters
      auto kp = pidATune.GetKp();
      auto ki = pidATune.GetKi();
      auto kd = pidATune.GetKd();
      pid.SetTunings(kp,ki,kd);
      AutoTuneHelper(false);
    }
  }
  else
    pid.Compute();

  UpdateRelayState();
  UpdateRampRate();
}


void ControllerState::UpdateRelayState()
{
  if (TargetTemperature == 0)
    relayControl.SetDutyCycle(0);
  else
    relayControl.SetDutyCycle(PIDOutput / 255.0);
}

void ControllerState::UpdateRampRate()
{
  unsigned long now = millis();
  unsigned long timeDelta = (now - LastUpdateTime);

  if (timeDelta == 0) timeDelta = 1;  // Avoid DIV0
  if (timeDelta > RAMPRATE_SAMPLING_INTERVAL)
  {
    auto timeInHours = (timeDelta / 1000.0) * 60 * 60;
    auto instantRampRate = (CurrentTemperature - lastTemperatureRead) / timeInHours;
    CurrentRampRate = rampRate.update(instantRampRate);
    LastUpdateTime = millis();
    lastTemperatureRead = CurrentTemperature;
  }
}

void ControllerState::ChangeAutoTune()
{
  if (!tuning)
  {
    pidATune.SetNoiseBand(aTuneNoise);
    pidATune.SetOutputStep(aTuneStep);
    pidATune.SetLookbackSec((int)aTuneLookBack);
    
    AutoTuneHelper(true);
    
    tuning = true;
  }
  else
  { 
    //cancel autotune
    pidATune.Cancel();
    tuning = false;
    AutoTuneHelper(false);
  }
}


void ControllerState::AutoTuneHelper(boolean start)
{
  if (start)
    ATuneModeRemember = pid.GetMode();
  else
    pid.SetMode(ATuneModeRemember);
}
