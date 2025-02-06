#ifndef ADAFRUIT_PM25AQI_I2C_BASE_H
#define ADAFRUIT_PM25AQI_I2C_BASE_H

#include "Adafruit_PM25AQI_Base.h"
#include <Adafruit_I2CDevice.h>

class Adafruit_PM25AQI_I2C_Base : public Adafruit_PM25AQI_Base {
protected:
  Adafruit_I2CDevice *i2c_dev = nullptr;
  virtual bool verify_starting_bytes(uint8_t *buffer) = 0;
  virtual bool verify_checksum(uint8_t *buffer, size_t bufLen) = 0;
  virtual void decode_data(uint8_t *buffer, PM25_AQI_Data *data) = 0;
};

#endif
