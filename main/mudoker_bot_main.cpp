#include "assets/faces.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal/i2c_types.h"
#include "i2c/i2c.hpp"
#include "screen/screen.hpp"
#include "servo/servo.hpp"
#include "soc/clk_tree_defs.h"
#include "soc/gpio_num.h"

#include "driver/gpio.h"
#include "esp_rom_sys.h"

i2c_master_bus_handle_t i2c_bus_handler = nullptr;
I2CManager i2c_manager;
SSD1306Screen ssd13068_screen;
ServoManager servo_manager;

void show_face(int *face_index);
void set_all_servos_angle(int *angle, int *direction);

extern "C" void app_main(void) {
  i2c_manager.init_i2c_master_bus(I2C_CLK_SRC_DEFAULT, I2C_NUM_0, GPIO_NUM_2,
                                  GPIO_NUM_1, true);

  ssd13068_screen.init_lcd_panel(i2c_bus_handler);

  servo_manager.init_servo_timer();
  for (int i = 0; i < sizeof(servo_pins) / sizeof(servo_pins[0]); i++) {
    servo_manager.attach_servo(i);
  }

  int face_index = 0;
  int servo_angle = 0;
  int servo_direction = 1;
  int tick_count = 0;
  while (true) {
    if (tick_count % 10 == 0) {
      show_face(&face_index);
    }
    set_all_servos_angle(&servo_angle, &servo_direction);
    vTaskDelay(pdMS_TO_TICKS(100));
    tick_count++;
  }
}

void show_face(int *face_index) {
  if (*face_index < 0 || *face_index >= FACE_COUNT) {
    ESP_LOGE("MAIN", "Invalid face index: %d", *face_index);
    return;
  }

  ssd13068_screen.render_bitmap(0, 0, ALL_FACES[*face_index]);
  *face_index = (*face_index + 1) % FACE_COUNT;
}

void set_all_servos_angle(int *angle, int *direction) {
  for (int i = 0; i < SERVO_CNT; i++) {
    int offset_angle = *angle;
    servo_manager.set_servo_angle(i, offset_angle);
  }

  int step = 2;
  if (*direction > 0) {
    *angle += step;
    if (*angle >= 180) {
      *angle = 180;
      *direction = -1;
    }
  } else {
    *angle -= step;
    if (*angle <= 0) {
      *angle = 0;
      *direction = 1;
    }
  }
}
