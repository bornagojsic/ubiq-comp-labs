#ifndef DHT22_H
#define DHT22_H

#include <stdint.h>
#include "driver/gpio.h"

typedef struct
{
    gpio_num_t pin;
} dht22_t;

void dht22_init(dht22_t *dev, gpio_num_t pin);
int dht22_read(dht22_t *dev, float *temperature, float *humidity);

#endif