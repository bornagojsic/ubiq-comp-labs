#include "ntc.h"
#include <math.h>

// https://www.gotronic.fr/pj2-mf52type-1554.pdf

#define ADC_MAX 4095.0f
#define VREF 3.3f

#define R0 10000.0f

/* Python code to calculate Steinhart-Hart coefficients from datasheet values:

    import numpy as np

    temp_C = np.array([
        -30, -25, -20, -15, -10, -5,
        0, 5, 10, 15, 20, 25,
        30, 35, 40, 45, 50, 55,
        60, 65, 70, 75, 80, 85,
        90, 95, 100, 105, 110
    ])
    resistance = np.array([
        181700, 133300, 98880, 74100, 56060, 42800,
        32590, 25580, 20000, 15760, 12510, 10000,
        8048, 6518, 5312, 4354, 3588, 2974,
        2476, 2072, 1743, 1473, 1250, 1065,
        911, 782.4, 674.4, 583.6, 506.6
    ])

    temp_K = temp_C + 273.15

    lnR = np.log(resistance)

    # 1/T = A + B*ln(R) + C*(ln(R))^3
    Y = 1 / temp_K
    X = np.vstack([np.ones_like(lnR), lnR, lnR**3]).T
    coeffs, _, _, _ = np.linalg.lstsq(X, Y, rcond=None)
    A, B, C = coeffs
    print("Steinhart–Hart coefficients:")
    print(f"A = {A:.10e}")
    print(f"B = {B:.10e}")
    print(f"C = {C:.10e}")

*/

#define A 1.1185041013e-03f
#define B 2.3649567979e-04f
#define C 7.3461320173e-08f

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

float ntc_read_temperature(ntc_t *dev)
{
    int raw;
    adc_oneshot_read(dev->adc, dev->channel, &raw);

    float r = R0 * raw / (ADC_MAX - raw);

    float lnR = logf(r);
    float invT = A + B * lnR + C * lnR * lnR * lnR;
    float tempK = 1.0f / invT;

    return tempK - 273.15f;
}