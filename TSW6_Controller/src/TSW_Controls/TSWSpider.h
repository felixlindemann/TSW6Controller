/**
 * @file TSWSpider.h
 * @brief HTTP interface for communication with the Train Sim World (TSW) API.
 *
 * @details
 * Provides simple GET-based communication with a running TSW instance or proxy
 * to set and query controller values.
 *
 * Example:
 * @code
 *   TSWSpider spider;
 *   spider.begin("192.168.4.2");
 *   spider.setControllerValue("Throttle", 0.75f);
 * @endcode
 *
 * @author Felix Lindemann
 * @date 2025-10-26
 * @version 1.0
 *
 * @copyright
 * This code is part of the TSW Controller Project.
 * Licensed under the MIT License.
 */

#pragma once
#include <Arduino.h>
#include <WiFiClient.h>
#include <HTTPClient.h>

class TSWSpider {
private:
  String host;
  String dtgCommKey;
  uint16_t port = 31270;
  bool serverReachable = false;

public:
  void begin( );
  bool setControllerValue(const String &controller, float value);
  float getControllerValue(const String &controller);
  
  /**
   * Check if TSW server is reachable via TCP connect.
   * @param timeoutMs  Connection timeout in milliseconds (default: 1000)
   * @return true if server responds, false otherwise
   */
  bool isServerReachable(uint16_t timeoutMs = 1000);
  
  /**
   * Get last known reachability status (cached).
   */
  bool isOnline() const { return serverReachable; }
  
  // Getters for status display
  const String& getHost() const { return host; }
  uint16_t getPort() const { return port; }
};
