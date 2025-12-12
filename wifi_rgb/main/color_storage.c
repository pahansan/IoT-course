#include "color_storage.h"

#include "nvs.h"

#define NVS_NAMESPACE "storage"
#define NVS_KEY_COLOR "rgb"

static uint32_t pack_rgb(uint8_t r, uint8_t g, uint8_t b) {
  return ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
}

static void unpack_rgb(uint32_t v, uint8_t* r, uint8_t* g, uint8_t* b) {
  *r = (v >> 16) & 0xFF;
  *g = (v >> 8) & 0xFF;
  *b = v & 0xFF;
}

esp_err_t color_storage_save(uint8_t r, uint8_t g, uint8_t b) {
  nvs_handle_t h;
  esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &h);
  if (err != ESP_OK) return err;

  uint32_t packed = pack_rgb(r, g, b);
  err = nvs_set_u32(h, NVS_KEY_COLOR, packed);
  if (err == ESP_OK) err = nvs_commit(h);

  nvs_close(h);
  return err;
}

esp_err_t color_storage_load(uint8_t* r, uint8_t* g, uint8_t* b) {
  nvs_handle_t h;
  esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &h);
  if (err != ESP_OK) return err;

  uint32_t packed = 0;
  err = nvs_get_u32(h, NVS_KEY_COLOR, &packed);
  nvs_close(h);

  if (err == ESP_OK) {
    unpack_rgb(packed, r, g, b);
  }
  return err;
}
