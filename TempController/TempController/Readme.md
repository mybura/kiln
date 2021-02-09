# Overview
I2C and UART programmable temperature controller implemented on a Arduino Nano MCU.

Senses the current temperature using a MAX31855 amplifier. Originally designed to use a K type thermocouple up to 1200 °C.

Tested with a [Kanthal A-1](https://www.kanthal.com/en/products/material-datasheets/wire/resistance-heating-wire-and-resistance-wire/kanthal-a-1/) coil switched via a [Fotek SSR](https://cdn.sparkfun.com/datasheets/Components/General/SSR40DA.pdf) run in PWM mode.

# Building
Software Required:
- Arduino IDE 1.8.5
- Visual Studio Code
- USB Serial Drivers

See circuit for hardware.

# Using

## Single Applications

Just add the logic to specify the required temperature on the same chip hosting the PID code:

```
void setup() 
{
  ...  (controller setup code)

  // Set target temperature to 134.5 Deg C
  controllerState.SetTargetTemperature(134.5);
}
```

## Multi Chip Applications

See the example program to set a temperature over the I2C bus:

```
...
    i2cInterface->SetTargetTemperature(134.5);
...
```

or read it using:

```
...
    auto liveTemperature = i2cInterface->GetCurrentTemperature();
    auto programmedTemperature = i2cInterface->GetTargetTemperature();
...
```


# Acknowledgements

- [SoftI2CMaster ](https://github.com/felias-fogg/SoftI2CMaster)
- [MemoryFree](http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1213583720/15)
- Some random forum post for the original MAX31855 code.
- The guy that created SimpleMovingAverage
- [Microsoft Visual Studio Code](https://code.visualstudio.com/)
- [Arduino](https://www.arduino.cc/)

