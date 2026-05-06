#include "DS1307.h"

DS1307::DS1307(i2c_port_t port, gpio_num_t sda, gpio_num_t scl)
{
    m_i2cPort = port;
    m_sdaPin = sda;
    m_sclPin = scl;
}

void DS1307::begin()
{
    i2c_config_t config = {};

    config.mode = I2C_MODE_MASTER;
    config.sda_io_num = m_sdaPin;
    config.scl_io_num = m_sclPin;
    config.sda_pullup_en = GPIO_PULLUP_ENABLE;
    config.scl_pullup_en = GPIO_PULLUP_ENABLE;
    config.master.clk_speed = DS1307_I2C_FREQ;

    i2c_param_config(m_i2cPort, &config);
    i2c_driver_install(m_i2cPort, config.mode, 0, 0, 0);
}

uint8_t DS1307::decToBcd(uint8_t value)
{
    return ((value / 10) << 4) | (value % 10);
}

uint8_t DS1307::bcdToDec(uint8_t value)
{
    return ((value >> 4) * 10) + (value & 0x0F);
}

void DS1307::writeRegister(uint8_t reg, uint8_t value)
{
    uint8_t data[2] = {reg, value};

    i2c_master_write_to_device(
        m_i2cPort,
        DS1307_ADDRESS,
        data,
        2,
        pdMS_TO_TICKS(1000));
}

uint8_t DS1307::readRegister(uint8_t reg)
{
    uint8_t value = 0;

    i2c_master_write_read_device(
        m_i2cPort,
        DS1307_ADDRESS,
        &reg,
        1,
        &value,
        1,
        pdMS_TO_TICKS(1000));

    return value;
}

void DS1307::setTime(uint16_t year,
                     uint8_t month,
                     uint8_t day,
                     uint8_t dayOfWeek,
                     uint8_t hour,
                     uint8_t minute,
                     uint8_t second)
{
    uint8_t data[8];

    data[0] = 0x00;
    data[1] = decToBcd(second) & 0x7F;
    data[2] = decToBcd(minute);
    data[3] = decToBcd(hour);
    data[4] = decToBcd(dayOfWeek);
    data[5] = decToBcd(day);
    data[6] = decToBcd(month);
    data[7] = decToBcd(year - 2000);

    i2c_master_write_to_device(
        m_i2cPort,
        DS1307_ADDRESS,
        data,
        8,
        pdMS_TO_TICKS(1000));
}

DateTime DS1307::getTime()
{
    DateTime t;
    uint8_t reg = 0x00;
    uint8_t data[7];

    i2c_master_write_read_device(
        m_i2cPort,
        DS1307_ADDRESS,
        &reg,
        1,
        data,
        7,
        pdMS_TO_TICKS(1000));

    t.second = bcdToDec(data[0] & 0x7F);
    t.minute = bcdToDec(data[1]);
    t.hour = bcdToDec(data[2] & 0x3F);
    t.dayOfWeek = bcdToDec(data[3]);
    t.day = bcdToDec(data[4]);
    t.month = bcdToDec(data[5]);
    t.year = bcdToDec(data[6]) + 2000;

    return t;
}