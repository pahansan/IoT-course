#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "i2c_bus.h"
#include <stdio.h>

#define I2C_MASTER_SCL_IO GPIO_NUM_22
#define I2C_MASTER_SDA_IO GPIO_NUM_21
#define I2C_MASTER_FREQ_HZ 100000
#define BUTTON GPIO_NUM_0

void app_main(void) {

  gpio_set_direction(BUTTON, GPIO_MODE_INPUT);
  gpio_set_pull_mode(BUTTON, GPIO_FLOATING);

  i2c_config_t conf = {
      .mode = I2C_MODE_MASTER,
      .sda_io_num = I2C_MASTER_SDA_IO,
      .sda_pullup_en = GPIO_PULLUP_DISABLE,
      .scl_io_num = I2C_MASTER_SCL_IO,
      .scl_pullup_en = GPIO_PULLUP_DISABLE,
      .master.clk_speed = I2C_MASTER_FREQ_HZ,
  };

  uint8_t button = 0;

  for (;;) {
    if (!gpio_get_level(BUTTON))
      button = 1;
    if (button) {
      i2c_bus_handle_t i2c0_bus = i2c_bus_create(I2C_NUM_0, &conf);
      uint8_t addresses[127];
      int count = i2c_bus_scan(i2c0_bus, addresses, 127);
      printf("Scanning the bus...\r\n\r\n");
      printf("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\r\n");
      for (int i = 0; i < 128; i += 16) {
        printf("%02x: ", i);
        for (int j = 0; j < 16; j++) {
          fflush(stdout);
          int k;
          for (k = 0; k < count; k++) {
            if (addresses[k] == i + j) {
              printf("%02x ", addresses[k]);
              break;
            }
          }
          if (k == count) {
            printf("-- ");
          }
        }
        printf("\r\n");
      }
      i2c_bus_delete(&i2c0_bus);
      button = 0;
    }
  }
}
