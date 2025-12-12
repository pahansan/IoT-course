#pragma once
#include <stdint.h>

#include "esp_err.h"

esp_err_t color_storage_save(uint8_t r, uint8_t g, uint8_t b);
esp_err_t color_storage_load(uint8_t* r, uint8_t* g, uint8_t* b);
