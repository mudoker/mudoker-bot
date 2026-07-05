#include "servo.hpp"
#include "soc/gpio_num.h"

int servo_pins[SERVO_CNT] = {
    GPIO_NUM_1,  // Servo 1 (J3 Pin 5)
    GPIO_NUM_2,  // Servo 2 (J3 Pin 6)
    GPIO_NUM_42, // Servo 3 (J3 Pin 7)
    GPIO_NUM_41, // Servo 4 (J3 Pin 8)
    GPIO_NUM_40, // Servo 5 (J3 Pin 9)
    GPIO_NUM_39, // Servo 6 (J3 Pin 10)
    GPIO_NUM_38, // Servo 7 (J3 Pin 11)
    GPIO_NUM_37  // Servo 8 (J3 Pin 12)
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