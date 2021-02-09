#ifndef I2CInterface_H_
#define I2CInterface_H_

#include "Display.h"
#include "Input.h"

#define I2C_BUS_ID 55

// Max time to wait for data before a packet is considered malformed
#define PACKET_TIMEOUT 1

class I2CInterface
{
  public:

    I2CInterface(Display* display, Input *input) { this->display = display; this->input = input; };
    void Setup();
    void Update();

    void OnDataReceived(int bytesReceived);
    void OnDataRequested();
  private:
  
  volatile Display* display;
  volatile Input* input;
};

#endif

