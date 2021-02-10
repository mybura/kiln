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
#define KP 3
#define KI 3.5
#define KD 0.7

//#define SIMULATE_TEMPRETATURE_IN 1

class ControllerState {
 public:
  ControllerState();

  void Update();

  void SetTargetTemperature(double temperature) { TargetTemperature = temperature; }
  void SetTargetRampRate(double rate) { TargetRampRate = rate; }
  double GetTargetTemperature() { return TargetTemperature; }

  double GetCurrentTemperature() { return CurrentTemperature; }
  double GetTargetRampRate() { return TargetRampRate; }
  double GetCurrentRampRate() { return CurrentRampRate; }

  void StartTuning();

 private:
  ThermoAmp thermoAmp;
  PID pid;
  PID_ATune pidATune;
  SimpleMovingAverage rampRate;
  RelayControl relayControl;
  
  double CurrentTemperature;
  double TargetTemperature;
  double CurrentRampRate;
  double TargetRampRate;
  double PIDOutput;

  double lastTemperatureRead;
  unsigned long LastUpdateTime;

  bool tuning;
  byte ATuneModeRemember=2;
  double aTuneStep=50, aTuneNoise=1;
  unsigned int aTuneLookBack=20;

  void UpdateRampRate();
  void UpdateRelayState();
  
  void AutoTuneHelper(boolean start);
  void ChangeAutoTune();
};

#endif
