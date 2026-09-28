idf_component_register(SRCS "src/bat_button.c"
                       INCLUDE_DIRS "include"
                       REQUIRES mqtt_app gotham_protocol
                       PRIV_REQUIRES driver)
