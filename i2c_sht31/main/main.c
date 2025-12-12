#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "mpu6050.h"
#include <stdio.h>

#define I2C_MASTER_SCL_IO GPIO_NUM_22
#define I2C_MASTER_SDA_IO GPIO_NUM_21
#define I2C_MASTER_FREQ_HZ 100000
#define I2C_SHT31_ADDR 0x44

void app_main(void) {}
