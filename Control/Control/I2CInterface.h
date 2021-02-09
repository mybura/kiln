#ifndef I2CInterface_H_
#define I2CInterface_H_

#include "Input.h"

#define I2C_UI_BUS_ID 55
#define I2C_PID_BUS_ID 56
#define MAX_STRING_INDEX 20

// Max time to wait for data before a packet is considered malformed
#define PACKET_TIMEOUT 10

class I2CInterface
{
  public:

    void Setup();
    void Update() { return; };

    void FillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void FillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void PrintText(int16_t x, int16_t y, byte index);
    void PrintTextCenter(int16_t x, int16_t y, int16_t w, int16_t h, byte index);
    void SetTextValue(uint16_t index, const char* value);
    void SetTextStyle(uint16_t color, uint16_t backgroundColor, uint16_t size);
    void PrintFloat(int16_t x, int16_t y, float value);

    float GetCurrentTemperature();
    float GetCurrentRampRate();
    float GetTargetTemperature();
    float GetTargetRampRate();

    void SetTargetTemperature(float value);
    void SetTargetRampRate(float value);
    void DisablePIDWatchDog();

    InputEvent GetInputEvent();

    byte RemainingInputQueueSize = 0;

  private:
};

#endif