#include <stdio.h>
#include "driver/ledc.h"
#include "esp_err.h"

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc.h"
#include "soc/gpio_reg.h"

#define LEDC_TIMER              LEDC_TIMER_0 // Используем таймер 0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_GPIO        GPIO_NUM_2 // Задаем GPIO - встроенный светодиод на плате
#define LEDC_CHANNEL            LEDC_CHANNEL_0 // Используем канал 0
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT // Разрешение таймера для коэф. заполнения (13 бит)
#define LEDC_DUTY               4096 // Коэффициент заполнения 50%. (2 ** 13) * 50% = 4096
#define LEDC_FREQUENCY          4000 // Частота ШИМ в Гц (4 кГц)

void app_main(void)
{
    int delay = 50;
    // Конфигурируем таймер
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .duty_resolution  = LEDC_DUTY_RES,
        .timer_num        = LEDC_TIMER,
        .freq_hz          = LEDC_FREQUENCY,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Настраиваем канал ЩИМ LED-контролера
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LEDC_OUTPUT_GPIO,
        .duty           = 0, // Коэффициент заполнения 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

    // Задаем коэффициент заполнения
    for(;;) {
      int i = 1;
      for(; i < 8192; i *= 2) {
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, i));
        // Применяем новые значение к выполнению
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
        vTaskDelay(delay / portTICK_PERIOD_MS);
      }
      for(; i > 1; i /= 2) {
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, i));
        // Применяем новые значение к выполнению
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
        vTaskDelay(delay / portTICK_PERIOD_MS);
      }
    }
}