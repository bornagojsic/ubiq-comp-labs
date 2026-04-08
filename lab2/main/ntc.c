#include "ntc.h"
#include <math.h>

#define ADC_MAX 4095.0f
#define VREF 3.3f

#define R_FIXED 10000.0f
#define R0 10000.0f
#define BETA 3950.0f
#define T0 298.15f

void ntc_init(ntc_t *dev, adc_channel_t channel)
{
    dev->channel = channel;

    adc_oneshot_unit_init_cfg_t unit_cfg = {
        .unit_id = ADC_UNIT_1,
    };

    adc_oneshot_new_unit(&unit_cfg, &dev->adc);

    adc_oneshot_chan_cfg_t chan_cfg = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_12,
    };

    adc_oneshot_config_channel(dev->adc, channel,
                               &chan_cfg);
}

// TODO: Use the Steinhart-Hart equation for better accuracy
float ntc_read_temperature(ntc_t *dev)
{
    int raw;
    adc_oneshot_read(dev->adc, dev->channel, &raw);

    float v = (raw / ADC_MAX) * VREF;

    if (v < 0.001f)
        v = 0.001f;
    if (v > VREF - 0.001f)
        v = VREF - 0.001f;

    float r = R_FIXED * v / (VREF - v);

    float tempK = 1.0f / (1.0f / T0 + (1.0f / BETA) * logf(r / R0));

    return tempK - 273.15f;
}