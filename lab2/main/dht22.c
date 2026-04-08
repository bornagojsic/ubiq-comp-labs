#include "dht22.h"
#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>

// --- Timing ---
#define START_DELAY_MS 20
#define RESPONSE_TIMEOUT 250
#define BIT_TIMEOUT 300
#define BIT_THRESHOLD 7

static int wait_for_level(gpio_num_t pin, int level, int timeout)
{
    int count = 0;
    while (gpio_get_level(pin) == level)
    {
        esp_rom_delay_us(2);
        if (++count > timeout)
            return -1;
    }
    return count;
}

void dht22_init(dht22_t *dev, gpio_num_t pin)
{
    dev->pin = pin;

    gpio_config_t cfg = {
        .pin_bit_mask = (1ULL << pin),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
    };
    gpio_config(&cfg);
}

int dht22_read(dht22_t *dev, float *temperature, float *humidity)
{
    uint8_t data[5] = {0};

    // Start signal
    gpio_set_direction(dev->pin, GPIO_MODE_OUTPUT);
    gpio_set_level(dev->pin, 0);
    vTaskDelay(pdMS_TO_TICKS(START_DELAY_MS));

    gpio_set_level(dev->pin, 1);
    esp_rom_delay_us(40);
    gpio_set_direction(dev->pin, GPIO_MODE_INPUT);

    // Response
    if (wait_for_level(dev->pin, 1, RESPONSE_TIMEOUT) < 0 ||
        wait_for_level(dev->pin, 0, RESPONSE_TIMEOUT) < 0 ||
        wait_for_level(dev->pin, 1, RESPONSE_TIMEOUT) < 0)
    {
        return -1;
    }

    // Read bits
    for (int i = 0; i < 40; i++)
    {
        if (wait_for_level(dev->pin, 0, BIT_TIMEOUT) < 0)
            return -1;

        int t = wait_for_level(dev->pin, 1, BIT_TIMEOUT);
        if (t < 0)
            return -1;

        data[i / 8] <<= 1;
        if (t > BIT_THRESHOLD)
            data[i / 8] |= 1;
    }

    // Checksum
    uint8_t sum = data[0] + data[1] + data[2] + data[3];
    if (sum != data[4])
        return -1;

    *humidity = ((data[0] << 8) | data[1]) / 10.0f;
    uint16_t raw_temp = (data[2] << 8) | data[3];

    if (raw_temp & 0x8000) // negative temperature
    {
        raw_temp &= 0x7FFF; // remove sign bit
        *temperature = -((float)raw_temp / 10.0f);
    }
    else
    {
        *temperature = (float)raw_temp / 10.0f;
    }

    return 0;
}