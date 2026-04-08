#include "button.h"

void button_init(button_t *btn, gpio_num_t pin)
{
    btn->pin = pin;

    gpio_config_t cfg = {
        .pin_bit_mask = (1ULL << pin),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
    };
    gpio_config(&cfg);
}

int button_is_pressed(button_t *btn)
{
    return gpio_get_level(btn->pin) == 0;
}