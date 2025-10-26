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

void TSWSpider::begin(const String &ip, uint16_t p) {
  host = ip;
  port = p;
  Serial.printf("[Spider] Initialized for host %s:%u\n", host.c_str(), port);
}

bool TSWSpider::setControllerValue(const String &controller, float value) {
  String url = "http://" + host + ":" + String(port) +
               "/set/ControllerValue/" + controller + "/" + String(value, 3);

  HTTPClient http;
  http.begin(url);
  int code = http.GET();
  http.end();

  Serial.printf("[Spider] %s -> %s (HTTP %d)\n",
                controller.c_str(), url.c_str(), code);
  return code == 200;
}

float TSWSpider::getControllerValue(const String &controller) {
  String url = "http://" + host + ":" + String(port) +
               "/get/CurrentDrivableActor/" + controller;

  HTTPClient http;
  http.begin(url);
  int code = http.GET();
  float val = 0.0f;
  if (code == 200) {
    String payload = http.getString();
    val = payload.toFloat(); // TSW returns numeric string
  }
  http.end();
  return val;
}
