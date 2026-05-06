#ifndef DS1307_H
#define DS1307_H

#include <stdint.h>
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define DS1307_ADDRESS 0x68
#define DS1307_I2C_FREQ 100000

struct DateTime
{
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t dayOfWeek;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
};

class DS1307
{
private:
  i2c_port_t m_i2cPort;
  gpio_num_t m_sdaPin;
  gpio_num_t m_sclPin;

  uint8_t decToBcd(uint8_t value);
  uint8_t bcdToDec(uint8_t value);

public:
  DS1307(i2c_port_t port, gpio_num_t sda, gpio_num_t scl);

  void begin();

  void setTime(uint16_t year,
               uint8_t month,
               uint8_t day,
               uint8_t dayOfWeek,
               uint8_t hour,
               uint8_t minute,
               uint8_t second);

  DateTime getTime();

  void writeRegister(uint8_t reg, uint8_t value);
  uint8_t readRegister(uint8_t reg);
};

#endif