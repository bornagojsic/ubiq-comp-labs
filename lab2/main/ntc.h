#ifndef NTC_H
#define NTC_H

#include "esp_adc/adc_oneshot.h"

typedef struct
{
    adc_oneshot_unit_handle_t adc;
    adc_channel_t channel;
} ntc_t;

void ntc_init(ntc_t *dev, adc_channel_t channel);
float ntc_read_temperature(ntc_t *dev);

#endif