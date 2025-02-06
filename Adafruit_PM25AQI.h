/*!
 * @file Adafruit_PM25AQI.h
 *
 * This is the documentation for Adafruit's PM25 AQI driver for the
 * Arduino platform.  It is designed specifically to work with the
 * Adafruit PM25 air quality sensors: http://www.adafruit.com/products/4632
 *
 * These sensors use I2C or UART to communicate.
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * Written by Ladyada for Adafruit Industries.
 *
 * BSD license, all text here must be included in any redistribution.
 *
 */

#ifndef ADAFRUIT_PM25AQI_H
#define ADAFRUIT_PM25AQI_H

#include "Arduino.h"
#include <Adafruit_I2CDevice.h>

// the i2c address
#define PMSA003I_I2CADDR_DEFAULT 0x12 ///< PMSA003I has only one I2C address

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



/*!
 *  @brief  Base class that stores state and functions for 
 *          interacting with I2C/UART PM2.5 Air Quality Sensors
 */
class Adafruit_PM25_Base {
public:
  Adafruit_PM25_Base();
  virtual ~Adafruit_PM25_Base() = default;

  virtual bool read(PM25_AQI_Data *data);

  uint16_t pm25_aqi_us(float concentration);
  uint16_t pm25_aqi_china(float concentration);
  uint16_t pm100_aqi_us(float concentration);
  uint16_t pm100_aqi_china(float concentration);

private:
  float linear(uint16_t aqi_high, uint16_t aqi_low, float conc_high,
               float conc_low, float concentration);
  bool process_buffer(uint8_t *buffer, size_t bufLen, PM25_AQI_Data *data);
  bool validate_checksum(uint8_t *buffer, size_t bufLen, uint16_t sum);
  bool validate_starting_bytes(uint8_t *buffer, size_t bufLen);
  uint8_t buffer[32];
  size_t bufLen = sizeof(buffer);
};

class Adafruit_PM25_I2C : public Adafruit_PM25_Base {
public:
  Adafruit_PM25_I2C();
  virtual ~Adafruit_PM25_I2C();
  virtual bool begin_I2C(TwoWire *theWire = &Wire, uint8_t addr = PMSA003I_I2CADDR_DEFAULT);
  virtual bool begin_I2C() { return begin_I2C(&Wire); }
  virtual bool read(PM25_AQI_Data *data) override;

private:
  Adafruit_I2CDevice *i2c_dev = NULL;
};

/*! 
 *  @brief  Plantower PMSA003I Driver (https://adafru.it/4632)
 */
class Adafruit_PM25_I2C_PMSA003I : public Adafruit_PM25_I2C {
public:
  Adafruit_PM25_I2C_PMSA003I();
};

class Adafruit_PM25_UART : public Adafruit_PM25_Base {
public:
  Adafruit_PM25_UART();
  virtual ~Adafruit_PM25_UART() = default;
  bool begin_UART(Stream *theStream);
  virtual bool begin_UART() override { return false; } // Must use begin_UART(Stream*)
  virtual bool read(PM25_AQI_Data *data) override = 0; // Made pure virtual

private:
  Stream *serial_dev = NULL;
};

/*! 
 *  @brief  Plantower PMS5003 Driver (https://adafru.it/3686)
 */
class Adafruit_PM25_UART_PMS5003 : public Adafruit_PM25_UART {
public:
  Adafruit_PM25_UART_PMS5003();
  virtual bool read(PM25_AQI_Data *data) override;
};

/*!
 *  @brief  Cubit PM1006 Driver (Included in IKEA Vindriktning)
 */
class Adafruit_PM25AQI_UART_PM1006 : public Adafruit_PM25_UART {
public:
  Adafruit_PM25AQI_UART_PM1006();
  virtual bool read(PM25_AQI_Data *data) override;

private:
  uint8_t buffer[20];
  size_t bufLen = sizeof(buffer);
};

#endif
