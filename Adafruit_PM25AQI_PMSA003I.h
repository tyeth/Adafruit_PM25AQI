#ifndef ADAFRUIT_PM25AQI_PMSA003I_H
#define ADAFRUIT_PM25AQI_PMSA003I_H

#include "Adafruit_PM25AQI_I2C_Base.h"

#define PMSA003I_I2CADDR_DEFAULT 0x12

class Adafruit_PM25AQI_PMSA003I : public Adafruit_PM25AQI_I2C_Base {
public:
  // Adafruit_PM25AQI_PMSA003I();
  bool read(PM25_AQI_Data *data) override;

private:
  static const uint8_t BUFFER_LENGTH = 32;
  uint8_t _buffer[BUFFER_LENGTH];
};

#endif
