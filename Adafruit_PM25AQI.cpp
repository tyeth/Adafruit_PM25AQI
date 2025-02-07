/*!
 * @file Adafruit_PM25AQI.cpp
 *
 * @mainpage Adafruit PM2.5 air quality sensor driver
 *
 * @section intro_sec Introduction
 *
 * This is the documentation for Adafruit's PM2.5 AQI driver for the
 * Arduino platform.  It is designed specifically to work with the
 * Adafruit PM2.5 Air quality sensors: http://www.adafruit.com/products/4632
 *
 * This library also works with the Cubic PM1006 UART Air Quality Sensor.
 *
 * These sensors use I2C or UART to communicate.
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 *
 * @section author Author
 * Written by Ladyada for Adafruit Industries.
 * Modified by Brent Rubell for Adafruit Industries for use with Cubic PM1006
 * Air Quality Sensor.
 *
 * @section license License
 * BSD license, all text here must be included in any redistribution.
 *
 */

#include "Adafruit_PM25AQI.h"
#include "Adafruit_PM25AQI_Base.h"
#include "Adafruit_PM25AQI_PMSA003I.h"
#include "Adafruit_PM25AQI_UART_PM1006.h"
#include "Adafruit_PM25AQI_UART_PMS5003.h"
#include <math.h>

/*!
 *  @brief  Instantiates a new PM25AQI class
 */
Adafruit_PM25AQI::Adafruit_PM25AQI() {}

Adafruit_PM25AQI::~Adafruit_PM25AQI() {
  if (driver) {
    delete driver;
    driver = nullptr;
  }
}

/*!
 *  @brief  Setups the hardware and detects a valid PMSA003I. Initializes I2C.
 *  @param  theWire
 *          Optional pointer to I2C interface, otherwise use Wire
 *  @return True if PMSA003I found on I2C, False if something went wrong!
 */
bool Adafruit_PM25AQI::begin_I2C(TwoWire *theWire, uint8_t addr) {
  if (driver) {
    delete driver;
    driver = nullptr;
  }
  driver = new Adafruit_PM25AQI_PMSA003I();
  return driver->begin_I2C(theWire, addr);
}

/*!
 *  @brief  Setups the hardware and detects a valid UART PM2.5
 *  @param  theSerial
 *          Pointer to Stream (HardwareSerial/SoftwareSerial) interface
 *  @return True
 */
bool Adafruit_PM25AQI::begin_UART(Stream *theStream) {
  if (driver) {
    delete driver;
    driver = nullptr;
  }
  uint8_t retries = 0;
  while (!driver && retries < 32) {
    for (uint8_t i = 0; i < 32; i++) {
      if (theStream->available()) {
        if (theStream->peek() == 0x42) {
          PM25AQI_DEBUG_PRINTLN("Found PMS5003");
          driver = new Adafruit_PM25AQI_UART_PMS5003();
          return driver->begin_UART(theStream);
          break;
        } else if (theStream->peek() == 0x16) {
          PM25AQI_DEBUG_PRINTLN("Found PM1006");
          driver = new Adafruit_PM25AQI_UART_PM1006();
          return driver->begin_UART(theStream);
          break;
        } else {
          PM25AQI_DEBUG_PRINT("Skipping byte: ");
          PM25AQI_DEBUG_PRINTLN(theStream->peek(), 16);
          theStream->read();
        }
        PM25AQI_DEBUG_PRINTLN("Trying another packet");
      } else {
        PM25AQI_DEBUG_PRINTLN("No serial data available, retrying");
      }
      delay(100);
      retries++;
    }
  }
  return false;
}

/*!
 *  @brief  Setups the hardware and detects a valid UART PM2.5
 *  @param  data
 *          Pointer to PM25_AQI_Data that will be filled by read()ing
 *  @return True on successful read, false if timed out or bad data
 */
bool Adafruit_PM25AQI::read(PM25_AQI_Data *data) {
  if (!driver || !data) {
    return false;
  }
  return driver->read(data);
}

/*!
 *  @brief  Get AQI of PM2.5 in US standard
 *  @param  concentration
 *          the environmental concentration of pm2.5 in ug/m3
 *  @return AQI number. 0 to 500 for valid calculation. 99999 for out of range.
 */
uint16_t Adafruit_PM25AQI::pm25_aqi_us(float concentration) {
  float c;
  float AQI;
  c = (floor(10 * concentration)) / 10;
  if (c < 0)
    AQI = 0;
  else if (c >= 0 && c < 12.1f) {
    AQI = linear(50, 0, 12, 0, c);
  } else if (c >= 12.1f && c < 35.5f) {
    AQI = linear(100, 51, 35.4f, 12.1f, c);
  } else if (c >= 35.5f && c < 55.5f) {
    AQI = linear(150, 101, 55.4f, 35.5f, c);
  } else if (c >= 55.5f && c < 150.5f) {
    AQI = linear(200, 151, 150.4f, 55.5f, c);
  } else if (c >= 150.5f && c < 250.5f) {
    AQI = linear(300, 201, 250.4f, 150.5f, c);
  } else if (c >= 250.5f && c < 350.5f) {
    AQI = linear(400, 301, 350.4f, 250.5f, c);
  } else if (c >= 350.5f && c < 500.5f) {
    AQI = linear(500, 401, 500.4f, 350.5f, c);
  } else {
    AQI = 99999; //
  }
  return round(AQI);
}

/*!
 *  @brief  Get AQI of PM10 in US standard
 *  @param  concentration
 *          the environmental concentration of pm10 in ug/m3
 *  @return AQI number. 0 to 500 for valid calculation. 99999 for out of range.
 */
uint16_t Adafruit_PM25AQI::pm100_aqi_us(float concentration) {
  float c;
  float AQI;
  c = concentration;
  if (c < 0)
    AQI = 0;
  else if (c < 55) {
    AQI = linear(50, 0, 55, 0, c);
  } else if (c < 155) {
    AQI = linear(100, 51, 155, 55, c);
  } else if (c < 255) {
    AQI = linear(150, 101, 255, 155, c);
  } else if (c < 355) {
    AQI = linear(200, 151, 355, 255, c);
  } else if (c < 425) {
    AQI = linear(300, 201, 425, 355, c);
  } else if (c < 505) {
    AQI = linear(400, 301, 505, 425, c);
  } else if (c < 605) {
    AQI = linear(500, 401, 605, 505, c);
  } else {
    AQI = 99999; //
  }
  return round(AQI);
}

/*!
 *  @brief  Get AQI of PM2.5 in China standard
 *  @param  concentration
 *          the environmental concentration of pm2.5 in ug/m3
 *  @return AQI number. 0 to 500 for valid calculation. 99999 for out of range.
 */
uint16_t Adafruit_PM25AQI::pm25_aqi_china(float concentration) {
  float c;
  float AQI;
  c = concentration;
  if (c < 0)
    AQI = 0;
  else if (c <= 35) {
    AQI = linear(50, 0, 35, 0, c);
  } else if (c <= 75) {
    AQI = linear(100, 51, 75, 35, c);
  } else if (c <= 115) {
    AQI = linear(150, 101, 115, 75, c);
  } else if (c <= 150) {
    AQI = linear(200, 151, 150, 115, c);
  } else if (c <= 250) {
    AQI = linear(300, 201, 250, 150, c);
  } else if (c <= 350) {
    AQI = linear(400, 301, 350, 250, c);
  } else if (c <= 500) {
    AQI = linear(500, 401, 500, 350, c);
  } else {
    AQI = 99999; //
  }
  return round(AQI);
}

/*!
 *  @brief  Get AQI of PM10 in China standard
 *  @param  concentration
 *          the environmental concentration of pm10 in ug/m3
 *  @return AQI number. 0 to 500 for valid calculation. 99999 for out of range.
 */
uint16_t Adafruit_PM25AQI::pm100_aqi_china(float concentration) {
  float c;
  float AQI;
  c = concentration;
  if (c < 0)
    AQI = 0;
  else if (c <= 50) {
    AQI = linear(50, 0, 50, 0, c);
  } else if (c <= 150) {
    AQI = linear(100, 51, 150, 50, c);
  } else if (c <= 250) {
    AQI = linear(150, 101, 250, 150, c);
  } else if (c <= 350) {
    AQI = linear(200, 151, 350, 250, c);
  } else if (c <= 420) {
    AQI = linear(300, 201, 420, 350, c);
  } else if (c <= 500) {
    AQI = linear(400, 301, 500, 420, c);
  } else if (c <= 600) {
    AQI = linear(500, 401, 600, 500, c);
  } else {
    AQI = 99999; //
  }
  return round(AQI);
}

/*!
 *  @brief  Linearly map a concentration value to its AQI level
 *  @param  aqi_high max aqi of the calculating range
 *  @param  aqi_low min aqi of the calculating range
 *  @param  conc_high max concentration value (ug/m3) of the calculating range
 *  @param  conc_low min concentration value (ug/m3) of the calculating range
 *  @param  concentration
 *          the concentration value to be calculated
 *  @return Calculated AQI value
 */
float Adafruit_PM25AQI::linear(uint16_t aqi_high, uint16_t aqi_low,
                               float conc_high, float conc_low,
                               float concentration) {
  float f;
  f = ((concentration - conc_low) / (conc_high - conc_low)) *
          (aqi_high - aqi_low) +
      aqi_low;
  return f;
}
