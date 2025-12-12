#include "cap_soil.h"
#include "rgb_led.h"

#include "esp_adc/adc_oneshot.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>

int cut(int raw) {
  if (raw >= 2700)
    return 1700;
  if (raw <= 1000)
    return 0;
  return raw - 1000;
}

double raw_to_voltage(int raw) { return (double)raw / 4095.0 * 3.3; }
double raw_to_percent(int raw) { return (double)(1700 - cut(raw)) / 17.0; }
int raw_to_green(int raw) {
  return (int)((double)(1700 - cut(raw)) * 0.15);
}
int raw_to_red(int raw) {
  return (int)((double)cut(raw) * 0.15);
}

void raw_to_led(int raw) {
  int cutted = cut(raw);
  rgb_led_set((int)((double)cutted * 0.15),
              (int)((double)(1700 - cutted) * 0.15), 0);
}


void app_main(void) {
  cap_soil_init();
  rgb_led_init();

  while (1) {
    int raw = cap_soil_read_raw();
    printf("ADC%d Channel[%d] Raw Data: %d, v: %f, %%: %f\n", 1, 4, raw,
           raw_to_voltage(raw), raw_to_percent(raw));
    rgb_led_set(raw_to_red(raw), raw_to_green(raw), 0);
    vTaskDelay(pdMS_TO_TICKS(10));
  }

  cap_soil_delete();
}
