#include "bat_button.h"
#include "esp_err.h"
#include "esp_log.h"
#include "gotham_protocol.h"
#include "heartbeat.h"
#include "mqtt_app.h"
#include "nvs_flash.h"
#include "wifi_sta.h"

static const char *TAG = "BAT_BUTTON_NODE";

void app_main(void)
{
    ESP_LOGI(TAG, "Bat-Button Node A | ESP32-S3 | BUTTON GPIO=%d | broker=%s | SSID=%s",
             BAT_BUTTON_GPIO, BAT_MQTT_BROKER_URI, BAT_WIFI_SSID);

    ESP_LOGI(TAG, "Inicializando NVS...");
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "Conectando ao Wi-Fi...");
    wifi_sta_start();

    ESP_LOGI(TAG, "Inicializando Botao Fisico...");
    bat_button_init();

    ESP_LOGI(TAG, "Iniciando cliente MQTT...");
    mqtt_app_start();

    bat_button_start();
    heartbeat_start();
}
