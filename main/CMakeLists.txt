idf_component_register(
    SRCS "mudoker_bot_main.cpp"
    INCLUDE_DIRS "."
    REQUIRES esp_driver_i2c spi_flash esp_lcd
)