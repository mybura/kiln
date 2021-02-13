#include "ControllerState.h"

#define RAMP_FILTER_MA_SAMPLES 10
#define RAMPRATE_SAMPLING_INTERVAL 2000

ControllerState::ControllerState()
{
    #ifdef SIMULATE_TEMPRETATURE_IN
  CurrentTemperature = 10;
    #else
  CurrentTemperature = 0;
    #endif
  tuning = false;
  TargetTemperature = 0;
  CurrentRampRate = 0;
  TargetRampRate = 0;
  lastTemperatureRead = 0;
  LastUpdateTime = millis();
  
  thermoAmp = new ThermoAmp(MAXCLK, MAXCS, MAXDO);
  pid = new PID(&CurrentTemperature, &PIDOutput, &TargetTemperature, KP, KI, KD, DIRECT);
  pid->SetOutputLimits(0, 255); // 255 = 100% duty cycle on PWM
  pid->SetMode(AUTOMATIC);
  pidATune = new PID_ATune(&CurrentTemperature, &PIDOutput);

  rampRate = new SimpleMovingAverage(RAMP_FILTER_MA_SAMPLES);  
  rampRate->begin();
  relayControl = new RelayControl();

  relayControl->Setup();
 
  // wait for MAX chip to stabilize
  delay(500);
}

void ControllerState::Update()
{
  // TODO Watchdog on Temp feedback and current used
  float c = thermoAmp->readFilteredCelsius();
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
    auto temperature = CurrentTemperature;
    Serial.print("Current Temperature: ");
    Serial.println(temperature);

    byte val = (pidATune->Runtime());
    if (PIDOutput < 0) PIDOutput = 0;
    if (PIDOutput > 255) PIDOutput = 255;

    Serial.print("Current Output: ");
    Serial.println(PIDOutput);

    if (val!=0)
    {
      Serial.println("Stopping Tuning.");
      tuning = false;
    }
    if(!tuning)
    { 
      Serial.println("Tuning done. Updating PID.");

      //we're done, set the tuning parameters
      auto kp = pidATune->GetKp();
      auto ki = pidATune->GetKi();
      auto kd = pidATune->GetKd();
    //  pid->SetTunings(kp,ki,kd);
      
      AutoTuneHelper(false);
     
      DisplayTuning();
    }
  }
  else
    pid->Compute();

  UpdateRelayState();
  UpdateRampRate();
}


void ControllerState::UpdateRelayState()
{
  if (TargetTemperature == 0)
    relayControl->SetDutyCycle(0);
  else
    relayControl->SetDutyCycle(PIDOutput / 255.0);
}

void ControllerState::StartTuning()
{
  Serial.println("Starting tuning.");
  tuning = false;
  ChangeAutoTune();
  tuning = true;
}

void ControllerState::DisplayTuning()
{
    auto kp = pidATune->GetKp();
    auto ki = pidATune->GetKi();
    auto kd = pidATune->GetKd();
  
    Serial.print("Tuner output: KP, KI, KD");
    Serial.print(kp);
    Serial.print(",");
    Serial.print(ki);
    Serial.print(",");
    Serial.println(kd);
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
    CurrentRampRate = rampRate->update(instantRampRate);
    LastUpdateTime = millis();
    lastTemperatureRead = CurrentTemperature;
  }
}
void ControllerState::ChangeAutoTune()
{
  if (!tuning)
  {
    PIDOutput = 255;
    TargetTemperature = CurrentTemperature;

    Serial.print("Tuning target temperature: ");
    Serial.println(TargetTemperature);

    pidATune->SetNoiseBand(aTuneNoise);
    pidATune->SetOutputStep(aTuneStep);
    pidATune->SetControlType(1); // PID
    pidATune->SetLookbackSec((int)aTuneLookBack);
    
    AutoTuneHelper(true);
    
    tuning = true;
  }
  else
  { 
    //cancel autotune
    pidATune->Cancel();
    tuning = false;
    AutoTuneHelper(false);
    TargetTemperature = 0;
  }
}

void ControllerState::AutoTuneHelper(boolean start)
{
  if (start)
    ATuneModeRemember = pid->GetMode();
  else
    pid->SetMode(ATuneModeRemember);
}
