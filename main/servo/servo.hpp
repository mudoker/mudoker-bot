#pragma once

#include "driver/mcpwm_gen.h"
#include "driver/mcpwm_prelude.h"
#include "driver/mcpwm_timer.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal/mcpwm_types.h"

#define SERVO_MIN_PULSE_WIDTH 500
#define SERVO_NEUTRAL_PULSE_WIDTH 1500
#define SERVO_MAX_PULSE_WIDTH 2500
#define PULSE_WIDTH_RANGE (SERVO_MAX_PULSE_WIDTH - SERVO_MIN_PULSE_WIDTH)
#define PULSE_WIDTH_BUFFER 100
#define SERVO_MAX_DEGREE 180
#define SERVO_CNT 2

extern int servo_pins[SERVO_CNT];
extern mcpwm_cmpr_handle_t pwm_comparator_handles[SERVO_CNT];

int convert_degree_to_pulse_width(int degree);

class ServoManager {
private:
  mcpwm_gen_handle_t pwm_generator_handles[SERVO_CNT] = {nullptr};
  mcpwm_timer_handle_t pwm_timer_handle = nullptr;
  inline void init_pwm_timer(int group_id, mcpwm_timer_clock_source_t clk_src,
                             int resolution_hz, int period_ticks,
                             mcpwm_timer_count_mode_t count_mode) {
    mcpwm_timer_config_t pwm_timer_config = {};
    memset(&pwm_timer_config, 0, sizeof(pwm_timer_config));
    pwm_timer_config.group_id = group_id;
    pwm_timer_config.clk_src = clk_src;
    pwm_timer_config.resolution_hz = resolution_hz;
    pwm_timer_config.period_ticks = period_ticks;
    pwm_timer_config.count_mode = count_mode;

    ESP_ERROR_CHECK(mcpwm_new_timer(&pwm_timer_config, &pwm_timer_handle));
  }

  inline mcpwm_oper_handle_t
  init_pwm_operator(mcpwm_timer_handle_t pwm_timer_handle, int group_id) {
    mcpwm_operator_config_t pwm_operator_config = {};
    pwm_operator_config.group_id = group_id;

    mcpwm_oper_handle_t pwm_operator_handle = nullptr;
    ESP_ERROR_CHECK(
        mcpwm_new_operator(&pwm_operator_config, &pwm_operator_handle));
    ESP_ERROR_CHECK(
        mcpwm_operator_connect_timer(pwm_operator_handle, pwm_timer_handle));

    return pwm_operator_handle;
  }

  inline mcpwm_gen_handle_t
  init_pwm_generator(mcpwm_oper_handle_t pwm_operator_handle, int servo_index) {
    mcpwm_generator_config_t pwm_generator_config = {};
    pwm_generator_config.gen_gpio_num = servo_pins[servo_index];

    mcpwm_gen_handle_t pwm_generator_handle = nullptr;
    ESP_ERROR_CHECK(mcpwm_new_generator(
        pwm_operator_handle, &pwm_generator_config, &pwm_generator_handle));
    return pwm_generator_handle;
  }

  inline mcpwm_cmpr_handle_t
  init_comparator(mcpwm_oper_handle_t pwm_operator_handle) {
    mcpwm_comparator_config_t pwm_comparator_config = {};
    pwm_comparator_config.flags.update_cmp_on_tez = true;

    mcpwm_cmpr_handle_t pwm_comparator_handle = nullptr;
    ESP_ERROR_CHECK(mcpwm_new_comparator(
        pwm_operator_handle, &pwm_comparator_config, &pwm_comparator_handle));
    return pwm_comparator_handle;
  }

public:
  virtual ~ServoManager() = default;
  void set_servo_angle(int servo_index, int angle);
  inline void init_servo_timer(int group_id, mcpwm_timer_clock_source_t clk_src,
                               int resolution_hz, int period_ticks,
                               mcpwm_timer_count_mode_t count_mode,
                               int servo_index = 0) {
    ESP_LOGI("SERVO", "Initialising Servo Group %d...", group_id);

    init_pwm_timer(group_id, clk_src, resolution_hz, period_ticks, count_mode);

    ESP_ERROR_CHECK(mcpwm_timer_enable(pwm_timer_handle));
    ESP_ERROR_CHECK(
        mcpwm_timer_start_stop(pwm_timer_handle, MCPWM_TIMER_START_NO_STOP));

    ESP_LOGI("SERVO", "Servo Timer Group %d Initialisation Success...",
             group_id);
  }

  inline void attach_servo(int group_id, int servo_index) {
    ESP_LOGI("SERVO", "Attaching Servo %d to Group %d...", servo_index,
             group_id);

    mcpwm_oper_handle_t pwm_operator_handle =
        init_pwm_operator(pwm_timer_handle, group_id);

    pwm_generator_handles[servo_index] =
        init_pwm_generator(pwm_operator_handle, servo_index);
    pwm_comparator_handles[servo_index] = init_comparator(pwm_operator_handle);

    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(
        pwm_generator_handles[servo_index],
        MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP,
                                     MCPWM_TIMER_EVENT_EMPTY,
                                     MCPWM_GEN_ACTION_HIGH)));

    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(
        pwm_generator_handles[servo_index],
        MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP,
                                       pwm_comparator_handles[servo_index],
                                       MCPWM_GEN_ACTION_LOW)));

    ESP_LOGI("SERVO", "Servo %d Attached to Group %d Successfully...",
             servo_index, group_id);
  }

  virtual void start_timer() {
    ESP_ERROR_CHECK(
        mcpwm_timer_start_stop(pwm_timer_handle, MCPWM_TIMER_START_NO_STOP));
  }

  virtual void stop_timer() {
    ESP_ERROR_CHECK(
        mcpwm_timer_start_stop(pwm_timer_handle, MCPWM_TIMER_STOP_EMPTY));
  }
};