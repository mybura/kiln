#ifndef _SIMPLE_MOVING_AVERAGE_H
#define _SIMPLE_MOVING_AVERAGE_H

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

class SimpleMovingAverage {
private:
  size_t _valuesIndex;
  size_t _usedLength;
  size_t _length;
  double _total;
  float* _values;
  


public:
  SimpleMovingAverage(size_t length = 10) : _length(length) {
    this->_values = new float[length];
  }

  ~SimpleMovingAverage() {
    delete[] this->_values;
  }

  void begin(void) {
    this->clear();
  }

  void clear(void) {
    this->_valuesIndex = 0;
    this->_usedLength = 0;
    this->_total = 0;

    for(uint8_t i=0; i < this->_length; i++) {
      this->_values[i] = 0;
    }
  }

  double update(float value) {
    this->_total -= this->_values[this->_valuesIndex];
    this->_total += value;
    this->_values[this->_valuesIndex] = value;

    this->_valuesIndex = (this->_valuesIndex + 1) % this->_length;
    if(this->_usedLength < this->_length) {
      this->_usedLength++;
    }

    return average();
  }

  float read(size_t index) {
    return this->_values[index];
  }

  float average(void) {
    if ((float)this->_usedLength == 0)
      return 0;
    else
      return this->_total / (float)this->_usedLength;
  }

  size_t length(void) {
    return this->_length;
  }
};

#endif

