idf_component_register(SRCS "src/wifi_sta.c"
                       INCLUDE_DIRS "include"
                       REQUIRES gotham_protocol
                       PRIV_REQUIRES esp_wifi esp_event esp_netif)
