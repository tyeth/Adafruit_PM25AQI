#ifndef ADAFRUIT_PM25AQI_BASE_H
#define ADAFRUIT_PM25AQI_BASE_H

#include "Adafruit_PM25AQI.h"

class Adafruit_PM25AQI_Base {
public:
  virtual ~Adafruit_PM25AQI_Base() {}
  virtual bool read(PM25_AQI_Data *data) = 0;
};

#endif
