#ifndef ADAFRUIT_PM25AQI_BASE_H
#define ADAFRUIT_PM25AQI_BASE_H

#include "Arduino.h"
#include <Adafruit_I2CDevice.h>

#if defined(PM25AQI_DEBUG)
#define PM25AQI_DEBUG_PRINT(...) Serial.print(__VA_ARGS__)
#define PM25AQI_DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)
#else
#define PM25AQI_DEBUG_PRINT(...)
#define PM25AQI_DEBUG_PRINTLN(...)
#endif

/**! Structure holding Plantower's standard packet **/
typedef struct PMSAQIdata {
  uint16_t framelen;       ///< How long this data chunk is
  uint16_t pm10_standard,  ///< Standard PM1.0
      pm25_standard,       ///< Standard PM2.5
      pm100_standard;      ///< Standard PM10.0
  uint16_t pm10_env,       ///< Environmental PM1.0
      pm25_env,            ///< Environmental PM2.5
      pm100_env;           ///< Environmental PM10.0
  uint16_t particles_03um, ///< 0.3um Particle Count
      particles_05um,      ///< 0.5um Particle Count
      particles_10um,      ///< 1.0um Particle Count
      particles_25um,      ///< 2.5um Particle Count
      particles_50um,      ///< 5.0um Particle Count
      particles_100um;     ///< 10.0um Particle Count
  uint16_t unused;         ///< Unused (version + error code)

  uint16_t checksum; ///< Packet checksum

  // AQI conversion results:
  uint8_t aqi_pm25_us;     ///< pm2.5 AQI of United States
  uint8_t aqi_pm100_us;    ///< pm10 AQI of United States
  uint8_t aqi_pm25_china;  ///< pm2.5 AQI of China
  uint8_t aqi_pm100_china; ///< pm10 AQI of China

} PM25_AQI_Data;


class Adafruit_PM25AQI_Base {
public:
  Adafruit_PM25AQI_Base();
  virtual ~Adafruit_PM25AQI_Base();
  virtual bool begin_I2C(TwoWire *theWire = &Wire, uint8_t addr = 0x12) = 0;
  virtual bool begin_UART(Stream *theStream) = 0;
  virtual bool read(PM25_AQI_Data *data) = 0;
  virtual bool verify_starting_bytes(uint8_t *buffer);
  virtual bool verify_checksum(uint8_t *buffer, size_t bufLen);
  virtual void decode_data(uint8_t *buffer, PM25_AQI_Data *data);

protected:
  uint8_t BUFFER_LENGTH = 32;
  uint8_t *_buffer;
};

#endif
