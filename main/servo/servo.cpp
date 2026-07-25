#include "servo.hpp"
#include "soc/gpio_num.h"

int servo_pins[SERVO_CNT] = {
    GPIO_NUM_4,  // Servo 1
    GPIO_NUM_5,  // Servo 2
    GPIO_NUM_6,  // Servo 3
    GPIO_NUM_7,  // Servo 4
    GPIO_NUM_10, // Servo 5
    GPIO_NUM_11, // Servo 6
    GPIO_NUM_12, // Servo 7
    GPIO_NUM_13  // Servo 8
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

  // Calculate duty cycle: inferring resolution and period from configuration
  uint32_t max_duty = 1 << duty_resolution;
  uint32_t period_us = MICROSECONDS_PER_SECOND / freq_hz;
  uint32_t duty = (pulse_width * max_duty) / period_us;

  ESP_ERROR_CHECK(ledc_set_duty(speed_mode, (ledc_channel_t)servo_index, duty));
  ESP_ERROR_CHECK(ledc_update_duty(speed_mode, (ledc_channel_t)servo_index));
}