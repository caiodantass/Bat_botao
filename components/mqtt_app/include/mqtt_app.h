#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void mqtt_app_start(void);
bool mqtt_app_is_connected(void);
int mqtt_app_publish(const char *topic, const char *payload);

#ifdef __cplusplus
}
#endif
