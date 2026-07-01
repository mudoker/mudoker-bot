#include "servo.hpp"
#include "soc/gpio_num.h"

int servo_pins[SERVO_CNT] = {GPIO_NUM_46, GPIO_NUM_9};
mcpwm_cmpr_handle_t pwm_comparator_handles[SERVO_CNT] = {nullptr};

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
  ESP_ERROR_CHECK(
      mcpwm_comparator_set_compare_value(pwm_comparator_handles[servo_index], pulse_width));
}