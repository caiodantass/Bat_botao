#include "heartbeat.h"

#include <stdio.h>

#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "gotham_protocol.h"
#include "mqtt_app.h"

static const char *TAG = "heartbeat";

static void bat_heartbeat_task(void *pvParameters)
{
    (void)pvParameters;
    char payload[100];

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(GOTHAM_HEARTBEAT_MS));

        if (!mqtt_app_is_connected()) {
            continue;
        }

        const int uptime = (int)(esp_timer_get_time() / 1000000);
        snprintf(payload, sizeof(payload),
                 "{\"device\": \"%s\", \"status\": \"ONLINE\", \"uptime_s\": %d}",
                 GOTHAM_DEVICE_ID, uptime);

        if (mqtt_app_publish(GOTHAM_TOPIC_STATUS, payload) < 0) {
            continue;
        }
        ESP_LOGI(TAG, "[HEARTBEAT] Status enviado para Batcaverna: %s", payload);
    }
}

void heartbeat_start(void)
{
    xTaskCreate(bat_heartbeat_task, "bat_heartbeat_task", 4096, NULL, 5, NULL);
}
