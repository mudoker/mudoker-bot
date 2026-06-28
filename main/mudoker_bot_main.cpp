#include "hal/i2c_types.h"
#include "i2c/i2c.hpp"
#include "soc/clk_tree_defs.h"
#include "soc/gpio_num.h"

void app_main(void) {
  I2CManager i2c_manager;

  i2c_manager.init_i2c_master_bus(I2C_CLK_SRC_DEFAULT, I2C_NUM_0, GPIO_NUM_4,
                                  GPIO_NUM_5, true);
}
