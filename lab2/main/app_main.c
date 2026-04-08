#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "soc/adc_channel.h"
#include "esp_adc/adc_oneshot.h"

#define ADC_CHANNEL ADC_CHANNEL_6 // GPIO34 on ADC1
#define VREF 3.3f
#define ADC_MAX 4095.0f

#define R_FIXED 10000.0f
#define BETA 3950.0f
#define T0 298.15f
#define R0 10000.0f

void app_main(void)
{
    // 1) Create ADC oneshot handle
    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_cfg = {
        .unit_id = ADC_UNIT_1,
        .clk_src = (adc_oneshot_clk_src_t)0,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_cfg, &adc1_handle));

    // 2) Configure ADC channel
    adc_oneshot_chan_cfg_t chan_cfg = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_12,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL, &chan_cfg));

    while (1)
    {
        int raw = 0;
        esp_err_t ret = adc_oneshot_read(adc1_handle, ADC_CHANNEL, &raw);
        if (ret != ESP_OK)
        {
            ESP_LOGE("NTC", "ADC read failed: %s", esp_err_to_name(ret));
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }

        // ADC → voltage → resistance → temperature (Beta equation)
        float voltage = ((float)raw / ADC_MAX) * VREF;

        // Avoid division by zero
        const float epsilon = 0.001f;
        if (voltage > VREF - epsilon)
            voltage = VREF - epsilon;
        if (voltage < epsilon)
            voltage = epsilon;

        float r_ntc = R_FIXED * voltage / (VREF - voltage);
        float lnR = logf(r_ntc / R0);
        float inv_T = (1.0f / T0) + (1.0f / BETA) * lnR;
        float tempC = (1.0f / inv_T) - 273.15f;

        printf("Raw=%d  V=%.3fV  R=%.1fΩ  T=%.2f°C\n",
               raw, voltage, r_ntc, tempC);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}