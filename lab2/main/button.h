#ifndef BUTTON_H
#define BUTTON_H

#include "driver/gpio.h"

typedef struct
{
    gpio_num_t pin;
} button_t;

void button_init(button_t *btn, gpio_num_t pin);
int button_is_pressed(button_t *btn);

#endif