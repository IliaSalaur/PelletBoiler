set(requires "esp_adc esp_event freertos nvs_flash lvgl lvgl_uifiles DynamicConfig")

idf_component_register(SRCS "main.cpp" "App.cpp"
                    REQUIRES ${requires}
                    INCLUDE_DIRS ".")