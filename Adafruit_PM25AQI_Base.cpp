#include "Adafruit_PM25AQI_Base.h"


  bool Adafruit_PM25AQI_Base::verify_starting_bytes(uint8_t *buffer) {
    // PMSA003I uses same protocol as PMS5003
    return (buffer[0] == 0x42 && buffer[1] == 0x4D);
  }
  
  bool Adafruit_PM25AQI_Base::verify_checksum(uint8_t *buffer, size_t bufLen) {
    uint16_t sum = 0;
    for (uint8_t i = 0; i < 30; i++) {
      sum += buffer[i];
    }
    uint16_t check = buffer[30] << 8;
    check += buffer[31];
    return check == sum;
  }
  
  void Adafruit_PM25AQI_Base::decode_data(uint8_t *buffer, PM25_AQI_Data *data) {
    // The data comes in endian'd, this solves it so it works on all platforms
    uint16_t buffer_u16[15];
    for (uint8_t i = 0; i < 15; i++) {
      buffer_u16[i] = buffer[2 + i * 2 + 1];
      buffer_u16[i] += (buffer[2 + i * 2] << 8);
    }
    
    // Copy to data struct
    memcpy((void *)data, (void *)buffer_u16, 30);
    
    // Get checksum ready
    data->checksum = buffer[30] << 8;
    data->checksum += buffer[31];
  }