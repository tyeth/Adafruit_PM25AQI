#include "Adafruit_PM25AQI_UART_Base.h"

Adafruit_PM25AQI_UART_Base::~Adafruit_PM25AQI_UART_Base() {
    if (serial_dev) {
        delete serial_dev;
    }
}

bool Adafruit_PM25AQI_UART_Base::begin_UART(Stream *theStream) {
    if (serial_dev) {
        delete serial_dev;
    }
    serial_dev = theStream;
    return serial_dev != nullptr;
}

bool Adafruit_PM25AQI_UART_Base::read_uart_data(uint8_t *buffer, size_t bufLen) {
  if (!serial_dev || !serial_dev->available()) {
    return false;
  }

  // Skip until we find a potential start byte
  int skipped = 0;
  while ((skipped < 32) && (serial_dev->peek() != 0x42) && 
         (serial_dev->peek() != 0x16)) {
    serial_dev->read();
    skipped++;
    if (!serial_dev->available()) {
      return false;
    }
  }

  // Are there enough bytes to read?
  if (serial_dev->available() < bufLen) {
    return false;
  }

  // Read the data
  return serial_dev->readBytes(buffer, bufLen) == bufLen;
}

bool Adafruit_PM25AQI_UART_Base::read(PM25_AQI_Data *data) {
    Serial.println("Adafruit_PM25AQI_UART_Base::read - Buffer length: " + String(BUFFER_LENGTH));
  if (!data || !read_uart_data(_buffer, BUFFER_LENGTH)) {
    return false;
  }

  if (!verify_starting_bytes(_buffer) || !verify_checksum(_buffer, BUFFER_LENGTH)) {
    return false;
  }

  decode_data(_buffer, data);
  return true;
}
