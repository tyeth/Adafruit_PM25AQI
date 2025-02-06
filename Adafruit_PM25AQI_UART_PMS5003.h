#ifndef ADAFRUIT_PM25AQI_UART_PMS5003_H
#define ADAFRUIT_PM25AQI_UART_PMS5003_H

#include "Adafruit_PM25AQI_UART_Base.h"

class Adafruit_PM25AQI_UART_PMS5003 : public Adafruit_PM25AQI_UART_Base {
public:
  bool read(PM25_AQI_Data *data) override;

protected:
  bool verify_starting_bytes(uint8_t *buffer) override;
  bool verify_checksum(uint8_t *buffer, size_t bufLen) override;
  void decode_data(uint8_t *buffer, PM25_AQI_Data *data) override;
  
private:
  static const uint8_t BUFFER_LENGTH = 32;
  uint8_t _buffer[BUFFER_LENGTH];
};

#endif
