#ifndef THERMOAMP_H
#define THERMOAMP_H

#include "Arduino.h"
#include "SMAFilter.h"

#define SMA_DEPTH 14

class ThermoAmp {
 public:
  ThermoAmp(int8_t _sclk, int8_t _cs, int8_t _miso);
  ThermoAmp(int8_t _cs);

  void begin(void);
  double readInternal(void);
  double readCelsius(void);
  double readCorrectedCelsius(void);  // The response curve compensated temperature for a K type thermocouple
  double readFilteredCelsius(void);   // An SMA of the Celcius reading
  double readFarenheit(void);
  uint8_t readError();
  bool ThermoAmp::hasError();
  uint32_t spiread32(void);

 private:
  boolean initialized;
  SimpleMovingAverage filter;
  
  int8_t sclk, miso, cs;
};

#endif

