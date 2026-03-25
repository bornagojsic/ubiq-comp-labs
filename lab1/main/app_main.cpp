#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "CButton.h"

static const char *TAG = "MAIN";

#define TASK_TICK_MS 10
#define BUTTON_GPIO_PORT 4

void onButtonClicked()
{
    ESP_LOGI(TAG, "Button clicked!");
}

void onButtonLongPressed()
{
    ESP_LOGI(TAG, "Button long pressed!");
}

void onButtonDoubleClicked()
{
    ESP_LOGI(TAG, "Button double clicked!");
}

void button_task(void *pvParameters)
{
    ESP_LOGI(TAG, "Button task started on GPIO%d", BUTTON_GPIO_PORT);

    CButton button(BUTTON_GPIO_PORT);
    button.attachSingleClick(onButtonClicked);
    button.attachLongPress(onButtonLongPressed);
    button.attachDoubleClick(onButtonDoubleClicked);

    ESP_LOGI(TAG, "Button initialized. Waiting for clicks...");

    while (1)
    {
        button.tick();
        vTaskDelay(TASK_TICK_MS / portTICK_PERIOD_MS);
    }
}

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Started");

    xTaskCreate(button_task, "button_task", 4096, NULL, 2, NULL);

    ESP_LOGI(TAG, "Button task created");

    while (1)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}