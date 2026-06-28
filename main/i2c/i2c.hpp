#pragma once
#include "driver/i2c_master.h"
#include "esp_log.h"
#include <sys/types.h>

extern i2c_master_bus_config_t i2c_master_bus_config;

class I2CManager {
public:
  inline i2c_master_bus_config_t
  init_i2c_master_bus(i2c_clock_source_t clk_source, i2c_port_num_t i2c_port,
                      gpio_num_t sda_io_num, gpio_num_t scl_io_num,
                      bool is_enable_internal_pullup,
                      uint8_t glitch_ignore_cnt = 7) {
    ESP_LOGI("MAIN", "Initialising I2C Port %d...", i2c_port);
    i2c_master_bus_config = {};

    i2c_master_bus_config.i2c_port = i2c_port;
    i2c_master_bus_config.sda_io_num = sda_io_num;
    i2c_master_bus_config.scl_io_num = scl_io_num;
    i2c_master_bus_config.clk_source = clk_source;
    i2c_master_bus_config.glitch_ignore_cnt = glitch_ignore_cnt;
    i2c_master_bus_config.intr_priority = 1;

    i2c_master_bus_config.flags.enable_internal_pullup =
        is_enable_internal_pullup;
    i2c_master_bus_config.flags.allow_pd = false;

    ESP_LOGI("MAIN", "I2C Port %d Configuration Success...", i2c_port);
    return i2c_master_bus_config;
  }
};
