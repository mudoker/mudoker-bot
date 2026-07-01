#pragma once
#include "assets/faces.hpp"
#include "driver/i2c_types.h"
#include "esp_err.h"
#include "esp_lcd_io_i2c.h"
#include "esp_lcd_panel_dev.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_ssd1306.h"
#include "esp_lcd_types.h"
#include "soc/gpio_num.h" 

extern esp_lcd_panel_handle_t panel_handler;

class ScreenManager {
protected:
  int i2c_address = 0x00;
  size_t control_phase_bytes = 0;
  size_t lcd_cmd_bits = 0;
  size_t lcd_param_bits = 0;
  unsigned int dc_bit_offset = 0;
  gpio_num_t reset_gpio_num = GPIO_NUM_NC;
  uint32_t scl_speed_hz = 400000;
  unsigned int bits_per_pixel = 1;

  inline esp_lcd_panel_io_i2c_config_t init_io_i2c_config() {
    esp_lcd_panel_io_i2c_config_t io_config = {};
    io_config.dev_addr = i2c_address;
    io_config.control_phase_bytes = control_phase_bytes;
    io_config.lcd_cmd_bits = lcd_cmd_bits;
    io_config.lcd_param_bits = lcd_param_bits;
    io_config.dc_bit_offset = dc_bit_offset;
    io_config.scl_speed_hz = scl_speed_hz;
    return io_config;
  }

  inline esp_lcd_panel_dev_config_t init_panel_config() {
    esp_lcd_panel_dev_config_t panel_config = {};
    panel_config.bits_per_pixel = bits_per_pixel;
    panel_config.reset_gpio_num = reset_gpio_num;
    return panel_config;
  }

public:
  int display_width = 128;
  int display_height = 64;

  virtual ~ScreenManager() = default;

  inline void init_lcd_panel(i2c_master_bus_handle_t i2c_handler) {
    esp_lcd_panel_io_i2c_config_t io_config = init_io_i2c_config();

    esp_lcd_panel_io_handle_t io_handler = nullptr;
    
    ESP_ERROR_CHECK(
        esp_lcd_new_panel_io_i2c(i2c_handler, &io_config, &io_handler));

    esp_lcd_panel_dev_config_t panel_config = init_panel_config();
    
    ESP_ERROR_CHECK(
        esp_lcd_new_panel_ssd1306(io_handler, &panel_config, &panel_handler));

    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handler));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handler));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handler, true));
  }

  void render_bitmap(int offset_x, int offset_y, const BitmapAsset& asset);
};

class SSD1306Screen : public ScreenManager {
public:
  SSD1306Screen() {
    i2c_address = 0x3C;
    control_phase_bytes = 1;
    lcd_cmd_bits = 8;
    lcd_param_bits = 8;
    dc_bit_offset = 6;
    reset_gpio_num = GPIO_NUM_NC;
    bits_per_pixel = 1;
    display_width = 128;
    display_height = 64;
  }
};