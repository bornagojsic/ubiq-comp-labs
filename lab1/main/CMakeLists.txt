idf_component_register(
    SRCS "app_main.cpp" "CButton.cpp" # "CLed.cpp"
    INCLUDE_DIRS "."
    REQUIRES driver esp_timer esp_driver_gpio
    PRIV_REQUIRES spi_flash
)