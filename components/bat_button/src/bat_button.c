#include "bat_button.h"

#include <stdbool.h>

#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "gotham_protocol.h"
#include "mqtt_app.h"

static const char *TAG = "BAT_BUTTON_NODE";
static bool s_bat_signal_state;

static void bat_button_task(void *pvParameters)
{
    (void)pvParameters;
    int last_state = 1;

    while (1) {
        int current_state = gpio_get_level((gpio_num_t)BAT_BUTTON_GPIO);

        if (current_state == 0 && last_state == 1) {
            vTaskDelay(pdMS_TO_TICKS(50));
            if (gpio_get_level((gpio_num_t)BAT_BUTTON_GPIO) == 0) {
                s_bat_signal_state = !s_bat_signal_state;
                const char *payload = s_bat_signal_state ? GOTHAM_MSG_SIGNAL_ON : GOTHAM_MSG_SIGNAL_OFF;

                mqtt_app_publish(GOTHAM_TOPIC_SIGNAL, payload);
                ESP_LOGI(TAG, "[ALERTA] Comissario Gordon acionou o Bat-Button! Publicado: %s", payload);

                while (gpio_get_level((gpio_num_t)BAT_BUTTON_GPIO) == 0) {
                    vTaskDelay(pdMS_TO_TICKS(10));
                }
            }
        }
        last_state = current_state;
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void bat_button_init(void)
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BAT_BUTTON_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);
}

void bat_button_start(void)
{
    xTaskCreate(bat_button_task, "bat_button_task", 4096, NULL, 5, NULL);
}
