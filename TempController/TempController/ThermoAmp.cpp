#include "ThermoAmp.h"
#include <avr/pgmspace.h>

#include <stdlib.h>
#include <SPI.h>

ThermoAmp::ThermoAmp(int8_t _sclk, int8_t _cs, int8_t _miso) : filter (SMA_DEPTH) {
  sclk = _sclk;
  cs = _cs;
  miso = _miso;

  initialized = false;
}

ThermoAmp::ThermoAmp(int8_t _cs) : filter (SMA_DEPTH) {
  cs = _cs;
  sclk = miso = -1;

  initialized = false;
}

void ThermoAmp::begin(void) {
  //define pin modes
  pinMode(cs, OUTPUT);
  digitalWrite(cs, HIGH);

  if (sclk == -1) {
    // hardware SPI
    //start and configure hardware SPI
    SPI.begin();
  } else {
    pinMode(sclk, OUTPUT); 
    pinMode(miso, INPUT);
  }
  filter.begin();
  initialized = true;
}

double ThermoAmp::readInternal(void) {
  uint32_t v;

  v = spiread32();

  // ignore bottom 4 bits - they're just thermocouple data
  v >>= 4;

  // pull the bottom 11 bits off
  float internal = v & 0x7FF;
  // check sign bit!
  if (v & 0x800) {
    // Convert to negative value by extending sign and casting to signed type.
    int16_t tmp = 0xF800 | (v & 0x7FF);
    internal = tmp;
  }
  internal *= 0.0625; // LSB = 0.0625 degrees
  //Serial.print("\tInternal Temp: "); Serial.println(internal);
  return internal;
}

double ThermoAmp::readCelsius(void) {

  int32_t v;

  v = spiread32();
 
  if (v & 0x7) {
    // uh oh, a serious problem!
    return NAN; 
  }

  if (v & 0x80000000) {
    // Negative value, drop the lower 18 bits and explicitly extend sign bits.
    v = 0xFFFFC000 | ((v >> 18) & 0x00003FFFF);
  }
  else {
    // Positive value, just drop the lower 18 bits.
    v >>= 18;
  }
  //Serial.println(v, HEX);
  
  double centigrade = v;

  // LSB = 0.25 degrees C
  centigrade *= 0.25;
  
  return centigrade;
}

double ThermoAmp::readFilteredCelsius(void)
{
  auto result = readCorrectedCelsius();
  if (isnan(result))
    return filter.average();
  else
    return filter.update(result);
}

double ThermoAmp::readCorrectedCelsius(void)
{
   // MAX31855 thermocouple voltage reading in mV
   auto internalTemperature = readInternal();
   float thermocoupleVoltage = (readCelsius() - internalTemperature) * 0.041276;
   
   // MAX31855 cold junction voltage reading in mV
   float coldJunctionTemperature = internalTemperature;
   float coldJunctionVoltage = -0.176004136860E-01 +
      0.389212049750E-01  * coldJunctionTemperature +
      0.185587700320E-04  * pow(coldJunctionTemperature, 2.0) +
      -0.994575928740E-07 * pow(coldJunctionTemperature, 3.0) +
      0.318409457190E-09  * pow(coldJunctionTemperature, 4.0) +
      -0.560728448890E-12 * pow(coldJunctionTemperature, 5.0) +
      0.560750590590E-15  * pow(coldJunctionTemperature, 6.0) +
      -0.320207200030E-18 * pow(coldJunctionTemperature, 7.0) +
      0.971511471520E-22  * pow(coldJunctionTemperature, 8.0) +
      -0.121047212750E-25 * pow(coldJunctionTemperature, 9.0) +
      0.118597600000E+00  * exp(-0.118343200000E-03 * pow((coldJunctionTemperature-0.126968600000E+03), 2.0));
                        
                        
   // cold junction voltage + thermocouple voltage         
   float voltageSum = thermocoupleVoltage + coldJunctionVoltage;
   
   // calculate corrected temperature reading based on coefficients for 3 different ranges   
   float b0, b1, b2, b3, b4, b5, b6, b7, b8, b9, b10;
   if(thermocoupleVoltage < 0){
      b0 = 0.0000000E+00;
      b1 = 2.5173462E+01;
      b2 = -1.1662878E+00;
      b3 = -1.0833638E+00;
      b4 = -8.9773540E-01;
      b5 = -3.7342377E-01;
      b6 = -8.6632643E-02;
      b7 = -1.0450598E-02;
      b8 = -5.1920577E-04;
      b9 = 0.0000000E+00;
   }
   
   else if(thermocoupleVoltage < 20.644){
      b0 = 0.000000E+00;
      b1 = 2.508355E+01;
      b2 = 7.860106E-02;
      b3 = -2.503131E-01;
      b4 = 8.315270E-02;
      b5 = -1.228034E-02;
      b6 = 9.804036E-04;
      b7 = -4.413030E-05;
      b8 = 1.057734E-06;
      b9 = -1.052755E-08;
   }
   
   else if(thermocoupleVoltage < 54.886){
      b0 = -1.318058E+02;
      b1 = 4.830222E+01;
      b2 = -1.646031E+00;
      b3 = 5.464731E-02;
      b4 = -9.650715E-04;
      b5 = 8.802193E-06;
      b6 = -3.110810E-08;
      b7 = 0.000000E+00;
      b8 = 0.000000E+00;
      b9 = 0.000000E+00;
   }
   
   else {
      // TODO: handle error - out of range
      return NAN;
   }
   
   return b0 +
      b1 * voltageSum +
      b2 * pow(voltageSum, 2.0) +
      b3 * pow(voltageSum, 3.0) +
      b4 * pow(voltageSum, 4.0) +
      b5 * pow(voltageSum, 5.0) +
      b6 * pow(voltageSum, 6.0) +
      b7 * pow(voltageSum, 7.0) +
      b8 * pow(voltageSum, 8.0) +
      b9 * pow(voltageSum, 9.0);
}

uint8_t ThermoAmp::readError() {
  return spiread32() & 0x7;
}

bool ThermoAmp::hasError() {
  return (spiread32() & (0x1l << 16));
}

double ThermoAmp::readFarenheit(void) {
  float f = readCelsius();
  f *= 9.0;
  f /= 5.0;
  f += 32;
  return f;
}

uint32_t ThermoAmp::spiread32(void) { 
  int i;
  uint32_t d = 0;

  // backcompatibility!
  if (! initialized) {
    begin();
  }

  delay(1);
  digitalWrite(cs, LOW);
  delay(1);

  if(sclk == -1) {
    // hardware SPI

    SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));

    d = SPI.transfer(0);
    d <<= 8;
    d |= SPI.transfer(0);
    d <<= 8;
    d |= SPI.transfer(0);
    d <<= 8;
    d |= SPI.transfer(0);

    SPI.endTransaction();
  } else {
    // software SPI

    digitalWrite(sclk, LOW);
    delay(1);

    // Read the data register
    for (i=31; i>=0; i--) {
      digitalWrite(sclk, HIGH);
      delay(1);
      if (digitalRead(miso)) {
        d |= 1L << i;
      }
      
      digitalWrite(sclk, LOW);
      delay(1);
    }
  }

  digitalWrite(cs, HIGH);
  delay(1);
  
  //Serial.print("Raw Read 0x"); Serial.println(d, HEX);
  
  return d;
}

