idf_component_register(
    SRCS "DS1307.cpp" "app_main.cpp"
    INCLUDE_DIRS "."
    REQUIRES driver esp_timer esp_driver_gpio
    PRIV_REQUIRES spi_flash
)