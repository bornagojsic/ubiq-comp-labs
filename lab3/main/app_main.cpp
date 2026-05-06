#include <stdio.h>
#include <string>
#include "esp_log.h"
#include "DS1307.h"

#define TAG "Lab 3"

std::string daysOfWeek[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void printDateTime(const DateTime &dt)
{
    ESP_LOGI(TAG, "Time: %02d.%02d.%04d. %02d:%02d:%02d, %s",
             dt.day,
             dt.month,
             dt.year,
             dt.hour,
             dt.minute,
             dt.second,
             daysOfWeek[dt.dayOfWeek % 7].c_str());
}

void printDivider()
{
    printf("\n---------------------------------------------\n\n");
}

extern "C" void app_main(void)
{
    DS1307 rtc(I2C_NUM_0, GPIO_NUM_21, GPIO_NUM_22);

    ESP_LOGI(TAG, "Starting DS1307...");
    rtc.begin();
    ESP_LOGI(TAG, "DS1307 initialized.");

    printDivider();

    ESP_LOGI(TAG, "Setting time to: 01.01.2026. 12:35:10, Thursday...");
    rtc.setTime(2026, 1, 1, 3, 12, 35, 10);
    ESP_LOGI(TAG, "Time set successfully.");

    printDivider();

    ESP_LOGI(TAG, "Reading time from RTC...");
    DateTime time = rtc.getTime();
    printDateTime(time);

    printDivider();

    ESP_LOGI(TAG, "Reading seconds register 0x00...");
    uint8_t secondsRegister = rtc.readRegister(0x00);
    ESP_LOGI(TAG, "Seconds register 0x00 = 0x%02X", secondsRegister);

    printDivider();

    ESP_LOGI(TAG, "Setting RAM register 0x08 to value 0xAB...");
    rtc.writeRegister(0x08, 0xAB);
    ESP_LOGI(TAG, "RAM register 0x08 set successfully.\n");

    ESP_LOGI(TAG, "Reading the same register again...");
    uint8_t ramRegister = rtc.readRegister(0x08);
    ESP_LOGI(TAG, "RAM register 0x08 = 0x%02X", ramRegister);

    printDivider();

    ESP_LOGI(TAG, "Setting control register 0x07 to value 0x00...");
    rtc.writeRegister(0x07, 0x00);
    ESP_LOGI(TAG, "Control register 0x07 set successfully.\n");

    ESP_LOGI(TAG, "Reading the same register again...");
    uint8_t controlRegister = rtc.readRegister(0x07);
    ESP_LOGI(TAG, "Control register 0x07 = 0x%02X", controlRegister);

    printDivider();

    while (true)
    {
        time = rtc.getTime();

        printDateTime(time);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}