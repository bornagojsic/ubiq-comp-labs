idf_component_register(
    SRCS "app_main.c" 
    INCLUDE_DIRS "."
    REQUIRES driver esp_timer esp_driver_gpio nvs_flash bt esp_event esp_system
    PRIV_REQUIRES spi_flash
)