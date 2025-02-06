#ifndef ADAFRUIT_PM25AQI_I2C_BASE_H
#define ADAFRUIT_PM25AQI_I2C_BASE_H

#include "Adafruit_PM25AQI_Base.h"
#include <Adafruit_I2CDevice.h>

class Adafruit_PM25AQI_I2C_Base : public Adafruit_PM25AQI_Base {
public:
  bool begin_I2C(TwoWire *theWire, uint8_t addr) {
    if (i2c_dev) {
      delete i2c_dev;
    }
    i2c_dev = new Adafruit_I2CDevice(addr, theWire);
    return i2c_dev->begin();
  }
protected:
  Adafruit_I2CDevice *i2c_dev = nullptr;
};

#endif
