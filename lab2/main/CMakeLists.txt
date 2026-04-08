idf_component_register(
    SRCS "app_main.c"          # ✅ Must match your filename exactly
    INCLUDE_DIRS "."
    REQUIRES driver esp_adc esp_driver_gpio
    PRIV_REQUIRES freertos
)

target_link_libraries(${COMPONENT_LIB} PRIVATE m)