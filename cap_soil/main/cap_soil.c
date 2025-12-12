#include "include/cap_soil.h"

#include "esp_adc/adc_oneshot.h"
#include <sys/_intsup.h>

#define ADC_UNIT ADC_UNIT_1
#define ADC_CHANNEL ADC_CHANNEL_4
#define ADC_ATTENUATION ADC_ATTEN_DB_12

adc_oneshot_unit_handle_t adc_handle;

void cap_soil_init() {
  adc_oneshot_unit_init_cfg_t init_config = {
      .unit_id = ADC_UNIT,
  };
  ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc_handle));

  adc_oneshot_chan_cfg_t config = {
      .bitwidth = ADC_BITWIDTH_DEFAULT,
      .atten = ADC_ATTENUATION,
  };
  ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, ADC_CHANNEL, &config));
}

int cap_soil_read_raw() {
  static int adc_raw;
  ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, ADC_CHANNEL, &adc_raw));
  return adc_raw;
}

double cap_soil_read_voltage() {
  static int adc_raw;
  ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, ADC_CHANNEL, &adc_raw));
  double v = (double)adc_raw;
  return v / 4095.0 * 3.3;
}

void cap_soil_delete() { ESP_ERROR_CHECK(adc_oneshot_del_unit(adc_handle)); }
