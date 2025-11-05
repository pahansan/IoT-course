#include "driver/ledc.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>

#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_MODE LEDC_LOW_SPEED_MODE
#define LEDC_GPIO_R GPIO_NUM_33
#define LEDC_CHANNEL_R LEDC_CHANNEL_0
#define LEDC_GPIO_G GPIO_NUM_25
#define LEDC_CHANNEL_G LEDC_CHANNEL_1
#define LEDC_GPIO_B GPIO_NUM_26
#define LEDC_CHANNEL_B LEDC_CHANNEL_2

#define LEDC_DUTY_RES LEDC_TIMER_13_BIT
#define LEDC_FADE_TIME 1000

// Переобразование R, G или B каналов в коэффициент заполнения с учетом
// разрешающей способности
#define RGB_TO_DUTY(x) ((x) * (1 << LEDC_DUTY_RES) / 255)

void app_main(void) {
  ledc_timer_config_t ledc_timer = {
      .duty_resolution = LEDC_DUTY_RES,
      .freq_hz = 4000,
      .speed_mode = LEDC_MODE,
      .timer_num = LEDC_TIMER,
      .clk_cfg = LEDC_AUTO_CLK,
  };
  ledc_timer_config(&ledc_timer);

  ledc_channel_config_t ledc_channel_r = {.channel = LEDC_CHANNEL_R,
                                          .duty = 0,
                                          .gpio_num = LEDC_GPIO_R,
                                          .speed_mode = LEDC_MODE,
                                          .hpoint = 0,
                                          .timer_sel = LEDC_TIMER,
                                          .flags.output_invert = 0};
  ledc_channel_config_t ledc_channel_g = {.channel = LEDC_CHANNEL_G,
                                          .duty = 0,
                                          .gpio_num = LEDC_GPIO_G,
                                          .speed_mode = LEDC_MODE,
                                          .hpoint = 0,
                                          .timer_sel = LEDC_TIMER,
                                          .flags.output_invert = 0};
  ledc_channel_config_t ledc_channel_b = {.channel = LEDC_CHANNEL_B,
                                          .duty = 0,
                                          .gpio_num = LEDC_GPIO_B,
                                          .speed_mode = LEDC_MODE,
                                          .hpoint = 0,
                                          .timer_sel = LEDC_TIMER,
                                          .flags.output_invert = 0};
  ledc_channel_config(&ledc_channel_r);
  ledc_channel_config(&ledc_channel_g);
  ledc_channel_config(&ledc_channel_b);

  // Initialize fade service.
  ledc_fade_func_install(0);

  uint8_t state = 0;
  while (1) {
    switch (state++) {
    case 0: // Красный, rgb(255, 0, 0)
      ledc_set_fade_with_time(ledc_channel_r.speed_mode, ledc_channel_r.channel,
                              RGB_TO_DUTY(255), LEDC_FADE_TIME);
      ledc_set_fade_with_time(ledc_channel_g.speed_mode, ledc_channel_g.channel,
                              RGB_TO_DUTY(0), LEDC_FADE_TIME);
      ledc_set_fade_with_time(ledc_channel_b.speed_mode, ledc_channel_b.channel,
                              RGB_TO_DUTY(0), LEDC_FADE_TIME);
      break;
    case 1: // Оранжевый, rgb(255, 127, 0)
      ledc_set_fade_with_time(ledc_channel_r.speed_mode, ledc_channel_r.channel,
                              RGB_TO_DUTY(255), LEDC_FADE_TIME);
      ledc_set_fade_with_time(ledc_channel_g.speed_mode, ledc_channel_g.channel,
                              RGB_TO_DUTY(127), LEDC_FADE_TIME);
      ledc_set_fade_with_time(ledc_channel_b.speed_mode, ledc_channel_b.channel,
                              RGB_TO_DUTY(0), LEDC_FADE_TIME);
      break;
    case 2: // Желтый, rgb(255, 255, 0)
      ledc_set_fade_with_time(ledc_channel_r.speed_mode, ledc_channel_r.channel,
                              RGB_TO_DUTY(255), LEDC_FADE_TIME);
      ledc_set_fade_with_time(ledc_channel_g.speed_mode, ledc_channel_g.channel,
                              RGB_TO_DUTY(255), LEDC_FADE_TIME);
      ledc_set_fade_with_time(ledc_channel_b.speed_mode, ledc_channel_b.channel,
                              RGB_TO_DUTY(0), LEDC_FADE_TIME);
      break;
    case 3: // Зеленый , rgb(0, 255, 0)
      ledc_set_fade_with_time(ledc_channel_r.speed_mode, ledc_channel_r.channel,
                              RGB_TO_DUTY(0), LEDC_FADE_TIME);
      ledc_set_fade_with_time(ledc_channel_g.speed_mode, ledc_channel_g.channel,
                              RGB_TO_DUTY(255), LEDC_FADE_TIME);
      ledc_set_fade_with_time(ledc_channel_b.speed_mode, ledc_channel_b.channel,
                              RGB_TO_DUTY(0), LEDC_FADE_TIME);
      break;
    case 4: // Голубой, rgb(0, 255, 255)
      ledc_set_fade_with_time(ledc_channel_r.speed_mode, ledc_channel_r.channel,
                              RGB_TO_DUTY(0), LEDC_FADE_TIME);
      ledc_set_fade_with_time(ledc_channel_g.speed_mode, ledc_channel_g.channel,
                              RGB_TO_DUTY(255), LEDC_FADE_TIME);
      ledc_set_fade_with_time(ledc_channel_b.speed_mode, ledc_channel_b.channel,
                              RGB_TO_DUTY(255), LEDC_FADE_TIME);
      break;
    case 5: // Синий, rgb(0, 0, 255)
      ledc_set_fade_with_time(ledc_channel_r.speed_mode, ledc_channel_r.channel,
                              RGB_TO_DUTY(0), LEDC_FADE_TIME);
      ledc_set_fade_with_time(ledc_channel_g.speed_mode, ledc_channel_g.channel,
                              RGB_TO_DUTY(0), LEDC_FADE_TIME);
      ledc_set_fade_with_time(ledc_channel_b.speed_mode, ledc_channel_b.channel,
                              RGB_TO_DUTY(255), LEDC_FADE_TIME);
      break;
    case 6: // Фиолетовый, rgb(255, 0, 255)
      ledc_set_fade_with_time(ledc_channel_r.speed_mode, ledc_channel_r.channel,
                              RGB_TO_DUTY(255), LEDC_FADE_TIME);
      ledc_set_fade_with_time(ledc_channel_g.speed_mode, ledc_channel_g.channel,
                              RGB_TO_DUTY(0), LEDC_FADE_TIME);
      ledc_set_fade_with_time(ledc_channel_b.speed_mode, ledc_channel_b.channel,
                              RGB_TO_DUTY(255), LEDC_FADE_TIME);
      break;
    }
    state %= 7;
    ledc_fade_start(ledc_channel_r.speed_mode, ledc_channel_r.channel,
                    LEDC_FADE_WAIT_DONE);
    ledc_fade_start(ledc_channel_g.speed_mode, ledc_channel_g.channel,
                    LEDC_FADE_WAIT_DONE);
    ledc_fade_start(ledc_channel_b.speed_mode, ledc_channel_b.channel,
                    LEDC_FADE_WAIT_DONE);
  }
}