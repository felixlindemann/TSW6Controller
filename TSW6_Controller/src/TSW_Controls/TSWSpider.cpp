/**
 * @file TSWSpider.cpp
 * @brief Implementation of the TSWSpider communication interface.
 *
 * @details
 * Handles HTTP GET requests to the TSW internal API (Spider).
 * Expected endpoints:
 *   /set/ControllerValue/<Controller>/<Value>
 *   /get/CurrentDrivableActor/<Controller>
 *
 * @note
 * Designed for ESP32 / ESP8266 based controllers.
 *
 * @author Felix Lindemann
 * @date 2025-10-26
 * @version 1.0
 */

#include "TSWSpider.h"
#include "../config.h"
#include "../ConfigStore.h"

void TSWSpider::begin(const String &ip, uint16_t p) {
  // Load config first to get server settings
  loadConfig();
  
  // Use provided IP/port or fall back to config values
  host = ip.length() > 0 ? ip : String(cfg.server.host);
  port = p > 0 ? p : cfg.server.port;
  
  LOG_HTTP_INFO("Spider API initializing...\n");
  
  // Load dtgCommKey from config
  if (strlen(cfg.server.apiKey) > 0) {
    dtgCommKey = String(cfg.server.apiKey);
    LOG_HTTP_DEBUG("Loaded API key from config (length: %d)\n", dtgCommKey.length());
  } else {
    dtgCommKey = "r2SpKhypgdoIfJQkgbCdKnXV2mKbrAwAqug3A3K/UA8="; // fallback
    LOG_HTTP_WARN("Config not found - using default API key\n");
  }
  
  LOG_HTTP_INFO("Spider connected to %s:%u\n", host.c_str(), port);
}

bool TSWSpider::setControllerValue(const String &controller, float value) {
  String url = "http://" + host + ":" + String(port) 
                + controller + "?inputValue=" + String(value, 16);

  LOG_HTTP_TRACE("SET %s = %.4f\n", controller.c_str(), value);

  HTTPClient http;
  http.begin(url);
  http.addHeader("DTGCommKey", dtgCommKey);
  http.setTimeout(300); // Nur 300ms warten statt 5000ms default
  int code = http.GET();
  http.end();

  if (code == 200) {
    LOG_HTTP_DEBUG("HTTP OK: %s\n", controller.c_str());
  } else if (code > 0) {
    LOG_HTTP_WARN("HTTP %d: %s\n", code, controller.c_str());
  } else {
    LOG_HTTP_ERROR("Connection failed (%d): %s\n", code, controller.c_str());
  }

  return code == 200;
}

float TSWSpider::getControllerValue(const String &controller) {
  String getUrl = controller;
  getUrl.replace("set/", "get/");
  String url = "http://" + host + ":" + String(port) + "/" + getUrl;

  HTTPClient http;
  http.begin(url);
  http.addHeader("DTGCommKey", dtgCommKey);
  int code = http.GET();
  float val = 0.0f;
  if (code == 200) {
    String payload = http.getString();
    val = payload.toFloat(); // TSW returns numeric string
  }
  http.end();
  return val;
}
