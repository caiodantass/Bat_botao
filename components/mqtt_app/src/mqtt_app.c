#include "mqtt_app.h"

#include <stdlib.h>

#include "esp_event.h"
#include "esp_log.h"
#include "gotham_protocol.h"
#include "mqtt_client.h"

static const char *TAG = "mqtt_app";
static esp_mqtt_client_handle_t s_client;
static volatile bool s_connected;

static void mqtt_event_handler(void *handler_args, esp_event_base_t base,
                               int32_t event_id, void *event_data)
{
    (void)handler_args;
    (void)base;
    esp_mqtt_event_handle_t event = event_data;

    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        s_connected = true;
        ESP_LOGI(TAG, "Conectado ao Broker MQTT com sucesso!");
        break;
    case MQTT_EVENT_DISCONNECTED:
        s_connected = false;
        ESP_LOGI(TAG, "Desconectado do Broker MQTT.");
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "Mensagem publicada com ID=%d", event->msg_id);
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGE(TAG, "MQTT error");
        break;
    default:
        break;
    }
}

void mqtt_app_start(void)
{
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = BAT_MQTT_BROKER_URI,
    };

    s_client = esp_mqtt_client_init(&mqtt_cfg);
    if (s_client == NULL) {
        ESP_LOGE(TAG, "esp_mqtt_client_init failed");
        abort();
    }
    ESP_ERROR_CHECK(esp_mqtt_client_register_event(s_client, ESP_EVENT_ANY_ID,
                                                   mqtt_event_handler, NULL));
    ESP_ERROR_CHECK(esp_mqtt_client_start(s_client));
}

bool mqtt_app_is_connected(void)
{
    return s_connected && s_client != NULL;
}

int mqtt_app_publish(const char *topic, const char *payload)
{
    if (s_client == NULL) {
        return -1;
    }
    return esp_mqtt_client_publish(s_client, topic, payload, 0, 1, 0);
}
