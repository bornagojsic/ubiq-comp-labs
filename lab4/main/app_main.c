#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "sdkconfig.h"

#include "esp_bt.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "nimble/nimble_npl.h"
#include "host/ble_hs.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#define JMBAG "0036551896"
#define JMBAG_ZADNJE4 0x1896
#define DEVICE_NAME "BornaGojsic"

#define SERVICE_UUID JMBAG_ZADNJE4
#define CHAR1_UUID (JMBAG_ZADNJE4 + 1)
#define CHAR2_UUID (JMBAG_ZADNJE4 + 2)
#define CHAR3_UUID (JMBAG_ZADNJE4 + 3)

static const char *TAG = "BLE-Server";
uint8_t ble_addr_type;
uint16_t current_conn_handle = BLE_HS_CONN_HANDLE_NONE;

uint32_t counterBLE = 0;
uint32_t writeBLE = 1;
char readBLE[] = JMBAG;

uint16_t char1_handle;

void ble_app_advertise(void);

static int device_read(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    if (attr_handle == char1_handle + 3)
    {
        os_mbuf_append(ctxt->om, readBLE, strlen(readBLE));
        ESP_LOGI(TAG, "Klijent pročitao Karakteristiku 2 (JMBAG): %s", readBLE);
        return 0;
    }

    char buf[32];
    snprintf(buf, sizeof(buf), "%lu", counterBLE);
    os_mbuf_append(ctxt->om, buf, strlen(buf));
    return 0;
}

static int device_write(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    char str_val[16] = {0};
    int len = ctxt->om->om_len;
    if (len > 15)
        len = 15;

    os_mbuf_copydata(ctxt->om, 0, len, str_val);

    uint32_t primljeni_broj = strtoul(str_val, NULL, 10);

    if (primljeni_broj >= 1 && primljeni_broj <= 10)
    {
        writeBLE = primljeni_broj;
        ESP_LOGI(TAG, "Karakteristika 3 uspješno ažurirana! writeBLE = %lu", writeBLE);
        return 0;
    }

    ESP_LOGW(TAG, "Primljeni broj %lu je izvan raspona [1, 10]! Ignoriram.", primljeni_broj);
    return 0;
}

static const struct ble_gatt_svc_def gatt_svcs[] = {
    {.type = BLE_GATT_SVC_TYPE_PRIMARY,
     .uuid = BLE_UUID16_DECLARE(SERVICE_UUID),
     .characteristics = (struct ble_gatt_chr_def[]){

         {.uuid = BLE_UUID16_DECLARE(CHAR1_UUID),
          .flags = BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ,
          .val_handle = &char1_handle,
          .access_cb = device_read},

         {.uuid = BLE_UUID16_DECLARE(CHAR2_UUID),
          .flags = BLE_GATT_CHR_F_READ,
          .access_cb = device_read},

         {.uuid = BLE_UUID16_DECLARE(CHAR3_UUID),
          .flags = BLE_GATT_CHR_F_WRITE,
          .access_cb = device_write},

         {0}}},
    {0}};

void counter_task(void *pvParameter)
{
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));

        counterBLE += writeBLE;
        ESP_LOGI(TAG, "counterBLE = %lu (uvećano za %lu)", counterBLE, writeBLE);

        char notification_str[32];
        snprintf(notification_str, sizeof(notification_str), "%lu", counterBLE);

        if (current_conn_handle != BLE_HS_CONN_HANDLE_NONE)
        {
            struct os_mbuf *om = ble_hs_mbuf_from_flat(
                notification_str,
                strlen(notification_str));

            if (om != NULL)
            {
                int rc = ble_gatts_notify_custom(
                    current_conn_handle,
                    char1_handle,
                    om);

                if (rc != 0)
                {
                    ESP_LOGW(TAG,
                             "Notify nije uspio, rc=%d",
                             rc);
                }
            }
        }
    }
}

static int ble_gap_event(struct ble_gap_event *event, void *arg)
{
    switch (event->type)
    {
    case BLE_GAP_EVENT_CONNECT:
        if (event->connect.status == 0)
        {
            current_conn_handle = event->connect.conn_handle;
            ESP_LOGI(TAG, "Klijent povezan, conn_handle=%d",
                     current_conn_handle);
        }
        else
        {
            ESP_LOGI(TAG, "Povezivanje nije uspjelo");
            ble_app_advertise();
        }
        break;

    case BLE_GAP_EVENT_DISCONNECT:
        ESP_LOGI(TAG, "Klijent odspojen");

        current_conn_handle = BLE_HS_CONN_HANDLE_NONE;

        ble_app_advertise();
        break;

    case BLE_GAP_EVENT_ADV_COMPLETE:
        ESP_LOGI("GAP", "BLE GAP EVENT ADV COMPLETE");
        ble_app_advertise();
        break;

    default:
        break;
    }
    return 0;
}

void ble_app_advertise(void)
{
    struct ble_hs_adv_fields fields;
    memset(&fields, 0, sizeof(fields));

    const char *device_name = ble_svc_gap_device_name();
    fields.name = (uint8_t *)device_name;
    fields.name_len = strlen(device_name);
    fields.name_is_complete = 1;

    fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;

    ble_gap_adv_set_fields(&fields);

    struct ble_gap_adv_params adv_params;
    memset(&adv_params, 0, sizeof(adv_params));
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;

    ble_gap_adv_start(ble_addr_type, NULL, BLE_HS_FOREVER, &adv_params, ble_gap_event, NULL);
}

void ble_app_on_sync(void)
{
    ble_hs_id_infer_auto(0, &ble_addr_type);
    ble_app_advertise();
}

void host_task(void *param)
{
    nimble_port_run();
}

void app_main()
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    esp_err_t ret_nimble = nimble_port_init();
    if (ret_nimble != ESP_OK)
    {
        ESP_LOGE("MAIN", "Inicijalizacija NimBLE porta nije uspjela: %s", esp_err_to_name(ret_nimble));
        return;
    }

    ble_svc_gap_init();
    ble_svc_gatt_init();

    ble_svc_gap_device_name_set(DEVICE_NAME);

    ble_gatts_count_cfg(gatt_svcs);
    ble_gatts_add_svcs(gatt_svcs);

    ble_hs_cfg.sync_cb = ble_app_on_sync;

    nimble_port_freertos_init(host_task);

    xTaskCreate(counter_task, "counter_task", 2048, NULL, 5, NULL);
}