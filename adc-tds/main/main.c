#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_oneshot.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>

// Выбираем АЦП
#define ADC_UNIT ADC_UNIT_1
#define ADC_CHANNEL ADC_CHANNEL_4
#define ADC_ATTENUATION ADC_ATTEN_DB_12

// Температура измеряемой жидкости
#define TEMPERATURE 25

static bool adc_calibration_init(adc_unit_t unit, adc_channel_t channel,
                                 adc_atten_t atten,
                                 adc_cali_handle_t *out_handle);
static void adc_calibration_deinit(adc_cali_handle_t handle);

void app_main(void) {
  static int adc_raw;
  static int voltage;
  static float sensor_voltage;
  static float sensor_tds;

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
  ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, ADC_CHANNEL, &config));

  // Калибровка АЦП
  adc_cali_handle_t adc_calibration_handle = NULL;
  bool do_calibration = adc_calibration_init(
      ADC_UNIT, ADC_CHANNEL, ADC_ATTENUATION, &adc_calibration_handle);

  while (1) {
    // Чтение "сырых" данных
    ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, ADC_CHANNEL, &adc_raw));

    if (do_calibration) {
      // Если АЦП откалиброван, то вывести значение в милливольтах
      ESP_ERROR_CHECK(
          adc_cali_raw_to_voltage(adc_calibration_handle, adc_raw, &voltage));
      sensor_voltage = 1e-3 * voltage / (1.0 + 0.02 * (TEMPERATURE - 25.0));
      sensor_tds =
          (133.42 * sensor_voltage * sensor_voltage * sensor_voltage -
           255.86 * sensor_voltage * sensor_voltage + 857.39 * sensor_voltage) *
          0.5;
      printf("TDS Value = %.1f ppm\n", sensor_tds);
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }

  // Никогда не будет выполнено, но удаляем всё, что создали
  ESP_ERROR_CHECK(adc_oneshot_del_unit(adc_handle));
  if (do_calibration) {
    adc_calibration_deinit(adc_calibration_handle);
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
