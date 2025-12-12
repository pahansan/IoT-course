#include "webserver.h"

#include <stdlib.h>
#include <string.h>

#include "color_storage.h"
#include "esp_err.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "rgb_led.h"

static const char* TAG = "WEB";

// EMBED_FILES: data/page.html, data/style.css, data/script.js
extern const uint8_t _binary_page_html_gz_start[];
extern const uint8_t _binary_page_html_gz_end[];

extern const uint8_t _binary_style_css_gz_start[];
extern const uint8_t _binary_style_css_gz_end[];

extern const uint8_t _binary_script_js_gz_start[];
extern const uint8_t _binary_script_js_gz_end[];


static esp_err_t root_handler(httpd_req_t* req) {
  const uint8_t* data = _binary_page_html_gz_start;
  size_t size = _binary_page_html_gz_end - _binary_page_html_gz_start;
  httpd_resp_set_type(req, "text/html; charset=utf-8");
  httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
  httpd_resp_set_hdr(req, "Cache-Control",
                     "no-cache, no-store, must-revalidate");
  httpd_resp_set_hdr(req, "Pragma", "no-cache");
  httpd_resp_set_hdr(req, "Expires", "0");
  httpd_resp_send(req, (const char*)data, size);
  return ESP_OK;
}

static esp_err_t css_handler(httpd_req_t* req) {
  const uint8_t* data = _binary_style_css_gz_start;
  size_t size = _binary_style_css_gz_end - _binary_style_css_gz_start;
  httpd_resp_set_type(req, "text/css; charset=utf-8");
  httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
  httpd_resp_send(req, (const char*)data, size);
  return ESP_OK;
}

static esp_err_t js_handler(httpd_req_t* req) {
  const uint8_t* data = _binary_script_js_gz_start;
  size_t size = _binary_script_js_gz_end - _binary_script_js_gz_start;
  httpd_resp_set_type(req, "application/javascript; charset=utf-8");
  httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
  httpd_resp_send(req, (const char*)data, size);
  return ESP_OK;
}

static esp_err_t get_handler(httpd_req_t* req) {
  uint8_t r = 0, g = 0, b = 0;
  esp_err_t err = color_storage_load(&r, &g, &b);
  if (err != ESP_OK) {
    r = g = b = 0;
  }
  char buf[64];
  int len = snprintf(buf, sizeof(buf), "{\"r\":%u,\"g\":%u,\"b\":%u}", r, g, b);
  httpd_resp_set_type(req, "application/json; charset=utf-8");
  httpd_resp_send(req, buf, len);
  return ESP_OK;
}

static esp_err_t set_handler(httpd_req_t* req) {
  char query[64];
  char param[8];
  int r = 0, g = 0, b = 0;
  if (httpd_req_get_url_query_str(req, query, sizeof(query)) == ESP_OK) {
    if (httpd_query_key_value(query, "r", param, sizeof(param)) == ESP_OK)
      r = atoi(param);
    if (httpd_query_key_value(query, "g", param, sizeof(param)) == ESP_OK)
      g = atoi(param);
    if (httpd_query_key_value(query, "b", param, sizeof(param)) == ESP_OK)
      b = atoi(param);
  }
  if (r < 0) r = 0;
  if (r > 255) r = 255;
  if (g < 0) g = 0;
  if (g > 255) g = 255;
  if (b < 0) b = 0;
  if (b > 255) b = 255;
  rgb_led_set((uint8_t)r, (uint8_t)g, (uint8_t)b);
  color_storage_save((uint8_t)r, (uint8_t)g, (uint8_t)b);
  char resp[64];
  snprintf(resp, sizeof(resp), "OK R=%d G=%d B=%d", r, g, b);
  httpd_resp_set_type(req, "text/plain; charset=utf-8");
  httpd_resp_send(req, resp, strlen(resp));
  return ESP_OK;
}

void start_webserver(void) {
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  httpd_handle_t server = NULL;
  if (httpd_start(&server, &config) != ESP_OK) {
    ESP_LOGE(TAG, "Failed to start HTTP server");
    return;
  }
  httpd_uri_t root = {.uri = "/",
                      .method = HTTP_GET,
                      .handler = root_handler,
                      .user_ctx = NULL};
  httpd_register_uri_handler(server, &root);
  httpd_uri_t css = {.uri = "/style.css",
                     .method = HTTP_GET,
                     .handler = css_handler,
                     .user_ctx = NULL};
  httpd_register_uri_handler(server, &css);
  httpd_uri_t js = {.uri = "/script.js",
                    .method = HTTP_GET,
                    .handler = js_handler,
                    .user_ctx = NULL};
  httpd_register_uri_handler(server, &js);
  httpd_uri_t set = {.uri = "/set",
                     .method = HTTP_GET,
                     .handler = set_handler,
                     .user_ctx = NULL};
  httpd_register_uri_handler(server, &set);
  httpd_uri_t get = {.uri = "/get",
                     .method = HTTP_GET,
                     .handler = get_handler,
                     .user_ctx = NULL};
  httpd_register_uri_handler(server, &get);
  ESP_LOGI(TAG, "HTTP server started");
}
