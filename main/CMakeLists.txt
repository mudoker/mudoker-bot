idf_component_register(
    SRCS "mudoker_bot_main.cpp" "screen/screen.cpp" "assets/faces.cpp"
    INCLUDE_DIRS "."
    REQUIRES esp_driver_i2c spi_flash esp_lcd
)