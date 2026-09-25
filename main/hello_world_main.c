#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "mqtt_client.h"
#include "driver/gpio.h"
#include "esp_timer.h" 
 
// --- CONFIGURAÇÕES DA REDE E DO BROKER ---
#define EXAMPLE_ESP_WIFI_SSID      "MERCUSYS_7E02" 
#define EXAMPLE_ESP_WIFI_PASS      "70960594" 
#define MQTT_BROKER_URI            "mqtt://192.168.177:1883" // IP do Broker Mosquitto do laboratório

// --- CONFIGURAÇÃO DO BOTÃO ---
#define BUTTON_GPIO                GPIO_NUM_4 

// Event Group para sinalizar quando o Wi-Fi estiver conectado
static EventGroupHandle_t s_wifi_event_group;
#define WIFI_CONNECTED_BIT         BIT0

static const char *TAG = "BAT_BUTTON_NODE";
static esp_mqtt_client_handle_t client = NULL;
static bool bat_signal_state = false;

// Manipulador de eventos do Wi-Fi
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "Conexão Wi-Fi caída. Tentando reconectar...");
        esp_wifi_connect();
        xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Wi-Fi conectado! IP obtido: " IPSTR, IP2STR(&event->ip_info.ip));
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

// Inicialização da Conexão Wi-Fi (Modo Station)
static void wifi_init_sta(void) {
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_any_id));
    // Corrigido para IP_EVENT_STA_GOT_IP
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .password = EXAMPLE_ESP_WIFI_PASS,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_sta finalizado.");
    
    xEventGroupWaitBits(s_wifi_event_group,
                        WIFI_CONNECTED_BIT,
                        pdFALSE,
                        pdFALSE,
                        portMAX_DELAY);
}

// Manipulador de eventos do MQTT
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = event_data;
    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "Conectado ao Broker MQTT com sucesso!");
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "Desconectado do Broker MQTT.");
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "Mensagem publicada com ID=%d", event->msg_id);
            break;
        default:
            break;
    }
}

// Inicialização do Cliente MQTT
static void mqtt_app_start(void) {
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = MQTT_BROKER_URI,
    };
    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}

// Configuração do Botão Físico na GPIO
static void button_init(void) {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BUTTON_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
}

// Tarefa de Leitura do Botão e Envio do Comando Principal
void bat_button_task(void *pvParameters) {
    int last_state = 1;
    while (1) {
        int current_state = gpio_get_level(BUTTON_GPIO);
        
        if (current_state == 0 && last_state == 1) {
            vTaskDelay(50 / portTICK_PERIOD_MS);
            if (gpio_get_level(BUTTON_GPIO) == 0) {
                bat_signal_state = !bat_signal_state;
                const char *payload = bat_signal_state ? "BAT_SIGNAL_ON" : "BAT_SIGNAL_OFF";
                
                if (client != NULL) {
                    esp_mqtt_client_publish(client, "gotham/dpgc/batsignal", payload, 0, 1, 0);
                    ESP_LOGI(TAG, "[ALERTA] Comissário Gordon acionou o Bat-Button! Publicado: %s", payload);
                }
                
                while (gpio_get_level(BUTTON_GPIO) == 0) {
                    vTaskDelay(10 / portTICK_PERIOD_MS);
                }
            }
        }
        last_state = current_state;
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

// --- DESAFIO EXTRA: "Heartbeat da Batcaverna" (Keep Alive) ---
void bat_heartbeat_task(void *pvParameters) {
    while (1) {
        vTaskDelay(30000 / portTICK_PERIOD_MS);
        
        if (client != NULL) {
            int uptime = (int)(esp_timer_get_time() / 1000000);
            char payload[100];
            snprintf(payload, sizeof(payload), "{\"device\": \"bat_button\", \"status\": \"ONLINE\", \"uptime_s\": %d}", uptime);
            
            esp_mqtt_client_publish(client, "gotham/dpgc/status", payload, 0, 1, 0);
            ESP_LOGI(TAG, "[HEARTBEAT] Status enviado para Batcaverna: %s", payload);
        }
    }
}

void app_main(void) {
    ESP_LOGI(TAG, "Inicializando NVS...");
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "Conectando ao Wi-Fi...");
    wifi_init_sta();

    ESP_LOGI(TAG, "Inicializando Botão Físico...");
    button_init();

    ESP_LOGI(TAG, "Iniciando cliente MQTT...");
    mqtt_app_start();

    xTaskCreate(bat_button_task, "bat_button_task", 4096, NULL, 5, NULL);
    xTaskCreate(bat_heartbeat_task, "bat_heartbeat_task", 4096, NULL, 5, NULL);
}