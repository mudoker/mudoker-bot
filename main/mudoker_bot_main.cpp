#include "assets/faces.hpp"
#include "hal/i2c_types.h"
#include "i2c/i2c.hpp"
#include "screen/screen.hpp"
#include "soc/clk_tree_defs.h"
#include "soc/gpio_num.h"

i2c_master_bus_handle_t i2c_bus_handler = nullptr;

extern "C" void app_main(void) {
  I2CManager i2c_manager;
  SSD1306Screen ssd13068_screen;

  i2c_manager.init_i2c_master_bus(I2C_CLK_SRC_DEFAULT, I2C_NUM_0, GPIO_NUM_5,
                                  GPIO_NUM_4, true);

  ssd13068_screen.init_lcd_panel(i2c_bus_handler);
  ssd13068_screen.render_bitmap(0, 0, EXCITED_FACE);
}
