#include "include/rgb_led.h"

#include "driver/ledc.h"
#include "esp_err.h"

#define PIN_R 33
#define PIN_G 25
#define PIN_B 26

#define LEDC_MODE LEDC_LOW_SPEED_MODE
#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_RES LEDC_TIMER_8_BIT
#define LEDC_FREQ 5000

void rgb_led_set(uint8_t r, uint8_t g, uint8_t b) {
  ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_0, r));
  ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_0));

  ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_1, g));
  ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_1));

  ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_2, b));
  ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_2));
}

void rgb_led_init(void) {
  ledc_timer_config_t t = {.speed_mode = LEDC_MODE,
                           .timer_num = LEDC_TIMER,
                           .duty_resolution = LEDC_RES,
                           .freq_hz = LEDC_FREQ,
                           .clk_cfg = LEDC_AUTO_CLK};
  ESP_ERROR_CHECK(ledc_timer_config(&t));

  ledc_channel_config_t c = {
      .speed_mode = LEDC_MODE,
      .timer_sel = LEDC_TIMER,
      .intr_type = LEDC_INTR_DISABLE,
      .duty = 0,
      .hpoint = 0,
      .flags.output_invert = 1,
  };

  c.channel = LEDC_CHANNEL_0;
  c.gpio_num = PIN_R;
  ESP_ERROR_CHECK(ledc_channel_config(&c));
  c.channel = LEDC_CHANNEL_1;
  c.gpio_num = PIN_G;
  ESP_ERROR_CHECK(ledc_channel_config(&c));
  c.channel = LEDC_CHANNEL_2;
  c.gpio_num = PIN_B;
  ESP_ERROR_CHECK(ledc_channel_config(&c));

  rgb_led_set(0, 0, 0);
}
