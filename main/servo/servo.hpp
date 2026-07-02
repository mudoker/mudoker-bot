#pragma once

#include "driver/ledc.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal/ledc_types.h"

#define SERVO_MIN_PULSE_WIDTH 500
#define SERVO_NEUTRAL_PULSE_WIDTH 1500
#define SERVO_MAX_PULSE_WIDTH 2500
#define PULSE_WIDTH_RANGE (SERVO_MAX_PULSE_WIDTH - SERVO_MIN_PULSE_WIDTH)
#define PULSE_WIDTH_BUFFER 100
#define SERVO_MAX_DEGREE 180
#define SERVO_CNT 1

extern int servo_pins[SERVO_CNT];

int convert_degree_to_pulse_width(int degree);

class ServoManager {
public:
  virtual ~ServoManager() = default;
  void set_servo_angle(int servo_index, int angle);

  inline void init_servo_timer() {
    ESP_LOGI("SERVO", "Initialising LEDC Timer for Servo...");

    ledc_timer_config_t ledc_timer = {};
    ledc_timer.speed_mode       = LEDC_LOW_SPEED_MODE;
    ledc_timer.duty_resolution  = LEDC_TIMER_13_BIT;
    ledc_timer.timer_num        = LEDC_TIMER_0;
    ledc_timer.freq_hz          = 50;
    ledc_timer.clk_cfg          = LEDC_USE_APB_CLK;

    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));
    ESP_LOGI("SERVO", "LEDC Timer Initialised Successfully.");
  }

  inline void attach_servo(int servo_index) {
    if (servo_index < 0 || servo_index >= SERVO_CNT) {
      ESP_LOGE("SERVO", "Invalid servo index: %d", servo_index);
      return;
    }

    ESP_LOGI("SERVO", "Attaching Servo %d (Pin %d)...", servo_index, servo_pins[servo_index]);

    ledc_channel_config_t ledc_channel = {};
    ledc_channel.gpio_num       = servo_pins[servo_index];
    ledc_channel.speed_mode     = LEDC_LOW_SPEED_MODE;
    ledc_channel.channel        = (ledc_channel_t)servo_index;
    ledc_channel.timer_sel      = LEDC_TIMER_0;
    ledc_channel.duty           = 0;
    ledc_channel.hpoint         = 0;

    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
    ESP_LOGI("SERVO", "Servo %d Attached Successfully.", servo_index);
  }
};