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
  host = ip;
  port = p;
  
  // Load dtgCommKey from config.json
  if (loadConfig()) {
    dtgCommKey = String(cfg.apiKey);
    TRACE_PRINT("[Spider] Loaded dtgCommKey from config: %s\n", dtgCommKey.c_str());
  } else {
    dtgCommKey = "r2SpKhypgdoIfJQkgbCdKnXV2mKbrAwAqug3A3K/UA8="; // fallback
    TRACE_PRINT("[Spider] Using default dtgCommKey\n");
  }
  
  TRACE_PRINT("[Spider] Initialized for host %s:%u\n", host.c_str(), port);
}

bool TSWSpider::setControllerValue(const String &controller, float value) {
  String url = "http://" + host + ":" + String(port) 
                + controller + "?inputValue=" + String(value, 16);

  TRACE_PRINT("[Spider] Setting controller value: %s -> %f\n.    DTGCommKey: %s: url=%s\n", controller.c_str(), value, dtgCommKey.c_str(), url.c_str());
  HTTPClient http;
  http.begin(url);
  http.addHeader("DTGCommKey", dtgCommKey);
  int code = http.GET();
  http.end();


  #if TRACE_API_CALL
    TRACE_PRINT("[Spider] %s : %f -> %s (HTTP %d)\n",                controller.c_str(), value, url.c_str(), code);
  #endif

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
