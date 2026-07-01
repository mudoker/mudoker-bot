#include "servo.hpp"
#include "soc/gpio_num.h"

int servo_pins[SERVO_CNT] = {
    GPIO_NUM_18, // Servo 1 (Left side pin)
    GPIO_NUM_1,  // Servo 2 (Right side pin)
    GPIO_NUM_9,  // Servo 3 (Left side pin)
    GPIO_NUM_2,  // Servo 4 (Right side pin)
    GPIO_NUM_10, // Servo 5 (Left side pin)
    GPIO_NUM_21, // Servo 6 (Right side pin)
    GPIO_NUM_11, // Servo 7 (Left side pin)
    GPIO_NUM_47  // Servo 8 (Right side pin)
};

int convert_degree_to_pulse_width(int degree) {
  if (degree < 0) {
    degree = 0;
  } else if (degree > SERVO_MAX_DEGREE) {
    degree = SERVO_MAX_DEGREE;
  }

  int pulse_width =
      SERVO_MIN_PULSE_WIDTH + (PULSE_WIDTH_RANGE * degree) / SERVO_MAX_DEGREE;

  if (pulse_width == SERVO_MIN_PULSE_WIDTH) {
    return SERVO_MIN_PULSE_WIDTH + PULSE_WIDTH_BUFFER;
  }

  if (pulse_width == SERVO_MAX_PULSE_WIDTH) {
    return SERVO_MAX_PULSE_WIDTH - PULSE_WIDTH_BUFFER;
  }

  return pulse_width;
}

void ServoManager::set_servo_angle(int servo_index, int angle) {
  if (servo_index < 0 ||
      servo_index >= sizeof(servo_pins) / sizeof(servo_pins[0])) {
    ESP_LOGE("SERVO", "Invalid servo index: %d", servo_index);
    return;
  }

  ESP_LOGI("SERVO", "Setting Servo %d Angle to %d degrees...", servo_index, angle);
  int pulse_width = convert_degree_to_pulse_width(angle);

  uint32_t duty = (pulse_width * 8192) / 20000;

  ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)servo_index, duty));
  ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)servo_index));
}