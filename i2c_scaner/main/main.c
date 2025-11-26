#include "driver/i2c_master.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>

#define I2C_TIMEOUT_VALUE_MS (50)

void app_main(void) {
  i2c_master_bus_config_t i2c_mst_config = {
      .clk_source = I2C_CLK_SRC_DEFAULT,
      .i2c_port = I2C_NUM_0,
      .scl_io_num = GPIO_NUM_22,
      .sda_io_num = GPIO_NUM_21,
      .glitch_ignore_cnt = 7,
      .flags.enable_internal_pullup = false,
  };
  i2c_master_bus_handle_t bus_handle;
  ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

  printf("Scanning the bus...\r\n\r\n");
  uint8_t address;
  printf("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\r\n");
  for (int i = 0; i < 128; i += 16) {
    printf("%02x: ", i);
    for (int j = 0; j < 16; j++) {
      fflush(stdout);
      address = i + j;
      esp_err_t ret =
          i2c_master_probe(bus_handle, address, I2C_TIMEOUT_VALUE_MS);
      if (ret == ESP_OK)
        printf("%02x ", address);
      else if (ret == ESP_ERR_TIMEOUT)
        printf("UU ");
      else
        printf("-- ");
    }
    printf("\r\n");
  }
  i2c_del_master_bus(bus_handle);
}