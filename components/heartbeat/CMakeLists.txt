idf_component_register(SRCS "src/heartbeat.c"
                       INCLUDE_DIRS "include"
                       REQUIRES mqtt_app gotham_protocol
                       PRIV_REQUIRES esp_timer)
