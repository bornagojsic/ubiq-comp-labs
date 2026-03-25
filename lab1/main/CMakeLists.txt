idf_component_register(
    SRCS "app_main.cpp" "CLed.cpp" "CButton.cpp"
    INCLUDE_DIRS "."
    REQUIRES driver esp_timer esp_driver_gpio    # ← Add this!
    PRIV_REQUIRES spi_flash
)