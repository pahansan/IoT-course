#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_oneshot.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>

// Выбираем АЦП
#define ADC_UNIT ADC_UNIT_1
#define ADC_ATTENUATION ADC_ATTEN_DB_12

static bool adc_calibration_init(adc_unit_t unit, adc_channel_t channel,
                                 adc_atten_t atten,
                                 adc_cali_handle_t *out_handle);
static void adc_calibration_deinit(adc_cali_handle_t handle);

void app_main(void) {
  static int adc_raw_4;
  static int voltage_4;
  static int adc_raw_7;
  static int voltage_7;

  // Инициализация АЦП
  adc_oneshot_unit_handle_t adc_handle;
  adc_oneshot_unit_init_cfg_t init_config = {
      .unit_id = ADC_UNIT,
  };
  ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc_handle));

  // Настройка АЦП
  adc_oneshot_chan_cfg_t config = {
      .bitwidth = ADC_BITWIDTH_DEFAULT,
      .atten = ADC_ATTENUATION,
  };
  ESP_ERROR_CHECK(
      adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_4, &config));
  ESP_ERROR_CHECK(
      adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_7, &config));

  // Калибровка АЦП
  adc_cali_handle_t adc_calibration_handle_4 = NULL;
  adc_cali_handle_t adc_calibration_handle_7 = NULL;
  bool do_calibration_4 = adc_calibration_init(
      ADC_UNIT, ADC_CHANNEL_4, ADC_ATTENUATION, &adc_calibration_handle_4);
  bool do_calibration_7 = adc_calibration_init(
      ADC_UNIT, ADC_CHANNEL_7, ADC_ATTENUATION, &adc_calibration_handle_7);

  while (1) {
    ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, ADC_CHANNEL_4, &adc_raw_4));
    double v = (double)adc_raw_4;
    v = v / 4095.0 * 3.3;
    printf("ADC%d Channel[%d] Raw Data: %d, v: %f\n", ADC_UNIT + 1,
           ADC_CHANNEL_4, adc_raw_4, v);

    if (do_calibration_4) {
      ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc_calibration_handle_4,
                                              adc_raw_4, &voltage_4));
      printf("ADC%d Channel[%d] Calibrated Voltage: %d mV\n", ADC_UNIT + 1,
             ADC_CHANNEL_4, voltage_4);
    }
    ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, ADC_CHANNEL_7, &adc_raw_7));
    v = (double)adc_raw_7;
    v = v / 4095.0 * 3.3;
    printf("ADC%d Channel[%d] Raw Data : %d, v: % f\n", ADC_UNIT + 1,
           ADC_CHANNEL_7, adc_raw_7, v);

    if (do_calibration_7) {
      ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc_calibration_handle_7,
                                              adc_raw_7, &voltage_7));
      printf("ADC%d Channel[%d] Calibrated Voltage: %d mV\n", ADC_UNIT + 1,
             ADC_CHANNEL_7, voltage_7);
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }

  // Никогда не будет выполнено, но удаляем всё, что создали
  ESP_ERROR_CHECK(adc_oneshot_del_unit(adc_handle));
  if (do_calibration_4) {
    adc_calibration_deinit(adc_calibration_handle_4);
  }
  ESP_ERROR_CHECK(adc_oneshot_del_unit(adc_handle));
  if (do_calibration_7) {
    adc_calibration_deinit(adc_calibration_handle_7);
  }
}

/*---------------------------------------------------------------
        Калибровка АЦП
---------------------------------------------------------------*/
static bool adc_calibration_init(adc_unit_t unit, adc_channel_t channel,
                                 adc_atten_t atten,
                                 adc_cali_handle_t *out_handle) {
  adc_cali_handle_t handle = NULL;
  esp_err_t ret = ESP_FAIL;
  bool calibrated = false;

#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
  if (!calibrated) {
    printf("Calibration scheme is \"Curve Fitting\"\n");
    adc_cali_curve_fitting_config_t cali_config = {
        .unit_id = unit,
        .chan = channel,
        .atten = atten,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ret = adc_cali_create_scheme_curve_fitting(&cali_config, &handle);
    if (ret == ESP_OK) {
      calibrated = true;
    }
  }
#endif
#if ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
  if (!calibrated) {
    printf("Calibration scheme is \"Line Fitting\"\n");
    adc_cali_line_fitting_config_t cali_config = {
        .unit_id = unit,
        .atten = atten,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ret = adc_cali_create_scheme_line_fitting(&cali_config, &handle);
    if (ret == ESP_OK) {
      calibrated = true;
    }
  }
#endif

  *out_handle = handle;
  if (ret == ESP_OK) {
    printf("Calibration Success\n");
  } else if (ret == ESP_ERR_NOT_SUPPORTED || !calibrated) {
    printf("eFuse not burnt, skip software calibration\n");
  } else {
    printf("Invalid arg or no memory\n");
  }

  return calibrated;
}

static void adc_calibration_deinit(adc_cali_handle_t handle) {
#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
  printf("deregister \"Curve Fitting\" calibration scheme\n");
  ESP_ERROR_CHECK(adc_cali_delete_scheme_curve_fitting(handle));

#elif ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
  printf("deregister \"Line Fitting\" calibration scheme\n");
  ESP_ERROR_CHECK(adc_cali_delete_scheme_line_fitting(handle));
#endif
}