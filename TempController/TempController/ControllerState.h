#ifndef CONTROLLERSTATE_H
#define CONTROLLERSTATE_H

#include <SPI.h>
#include "ThermoAmp.h"
#include <PID_v1.h>
#include "PID_AutoTune_v0.h"
#include "RelayControl.h"

// Creating a thermocouple instance with software SPI on three
// digital IO pins.
#define MAXDO   8
#define MAXCS   11
#define MAXCLK  9

// PID values
#define KP 14.53
#define KI 0.3
#define KD 174.7


// PID tuning values
#define TUNE_STEP 50.0
#define TUNE_NOISE 1.0
#define TUNE_LOOPBACK_SEC 20

//#define SIMULATE_TEMPRETATURE_IN 1

class ControllerState {
 public:
  ControllerState();

  void Update();

  void StartTuning();
  void DisplayTuning();

  void SetTargetTemperature(double temperature) { TargetTemperature = temperature; }
  void SetTargetRampRate(double rate) { TargetRampRate = rate; }
  double GetTargetTemperature() { return TargetTemperature; }

  double GetCurrentTemperature() { return CurrentTemperature; }
  double GetTargetRampRate() { return TargetRampRate; }
  double GetCurrentRampRate() { return CurrentRampRate; }

 private:
  ThermoAmp* thermoAmp;
  PID* pid;
  PID_ATune* pidATune;
  SimpleMovingAverage* rampRate;
  RelayControl* relayControl;
  
  double CurrentTemperature;
  double TargetTemperature;
  double CurrentRampRate;
  double TargetRampRate;
  double PIDOutput;

  double lastTemperatureRead;
  unsigned long LastUpdateTime;

  bool tuning  = false;
  byte ATuneModeRemember=2;
  double aTuneStep=255, aTuneNoise=2;
  unsigned int aTuneLookBack = 20;
  void UpdateRampRate();
  void UpdateRelayState();
  void AutoTuneHelper(boolean start);
  void ChangeAutoTune();};

#endif
