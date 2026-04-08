#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "dht22.h"
#include "ntc.h"
#include "button.h"

#define DHT_PIN GPIO_NUM_15
#define BTN_PIN GPIO_NUM_13
#define ADC_CH ADC_CHANNEL_6

#define STARTUP_DELAY pdMS_TO_TICKS(900)
#define LOOP_DELAY pdMS_TO_TICKS(100)
#define BUTTON_POLL_DELAY pdMS_TO_TICKS(50)
#define POST_READ_DELAY pdMS_TO_TICKS(2000)

dht22_t dht;
ntc_t ntc;
button_t btn;

void loop(void)
{

    vTaskDelay(LOOP_DELAY);

    if (!button_is_pressed(&btn))
        return;

    printf("\nReading sensors...\n");

    float ntc_temp = ntc_read_temperature(&ntc);
    printf("NTC:\t%.1f°C\n", ntc_temp);

    float temp, hum;

    if (dht22_read(&dht, &temp, &hum) == 0)
        printf("DHT22:\t%.1f°C, %.1f%%RH\n", temp, hum);
    else
        printf("DHT22:\tReading error!\n");

    while (button_is_pressed(&btn))
        vTaskDelay(BUTTON_POLL_DELAY);

    vTaskDelay(POST_READ_DELAY);
}

void app_main(void)
{
    dht22_init(&dht, DHT_PIN);
    ntc_init(&ntc, ADC_CH);
    button_init(&btn, BTN_PIN);

    vTaskDelay(STARTUP_DELAY);

    printf("System ready\n");

    while (1)
        loop();
}