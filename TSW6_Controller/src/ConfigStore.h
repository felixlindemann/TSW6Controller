// ConfigStore.h
// =============================================================================
// Runtime Configuration - Loaded from /config.json on LittleFS
// =============================================================================
#pragma once

#include <Arduino.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "config.h"

// =============================================================================
// Configuration Structure
// =============================================================================

struct WifiConfig {
  char ssid[32];
  char password[64];
  bool apModePreferred;
  unsigned long apCheckInterval;
  unsigned long heartbeatInterval;
};

struct ServerConfig {
  char host[64];
  uint16_t port;
  char apiKey[128];
};

struct DeviceConfig {
  char name[32];
  char mdnsName[32];
};

struct Config {
  WifiConfig wifi;
  ServerConfig server;
  DeviceConfig device;
};

extern Config cfg;

// =============================================================================
// Load Configuration from JSON
// =============================================================================

inline bool loadConfig() {
  LOG_SYS_DEBUG("Loading config from /config.json...\n");
  
  if (!LittleFS.begin()) {
    LOG_SYS_ERROR("LittleFS mount failed\n");
    return false;
  }
  
  // Set defaults first
  memset(&cfg, 0, sizeof(cfg));
  
  // WiFi defaults
  strncpy(cfg.wifi.ssid, "", sizeof(cfg.wifi.ssid));
  strncpy(cfg.wifi.password, "", sizeof(cfg.wifi.password));
  cfg.wifi.apModePreferred = true;
  cfg.wifi.apCheckInterval = 1000;
  cfg.wifi.heartbeatInterval = 10000;
  
  // Server defaults
  strncpy(cfg.server.host, DEFAULT_TSW_HOST, sizeof(cfg.server.host));
  cfg.server.port = DEFAULT_TSW_PORT;
  strncpy(cfg.server.apiKey, "", sizeof(cfg.server.apiKey));
  
  // Device defaults
  strncpy(cfg.device.name, DEFAULT_DEVICE_NAME, sizeof(cfg.device.name));
  strncpy(cfg.device.mdnsName, DEFAULT_MDNS_NAME, sizeof(cfg.device.mdnsName));
  
  File f = LittleFS.open("/config.json", "r");
  if (!f) {
    LOG_SYS_WARN("Config file not found - using defaults\n");
    return false;
  }

  DynamicJsonDocument doc(2048);
  DeserializationError err = deserializeJson(doc, f);
  f.close();

  if (err) {
    LOG_SYS_ERROR("Config JSON parse error: %s\n", err.c_str());
    return false;
  }

  // --- WiFi Section ---
  JsonObject wifi = doc["wifi"];
  if (wifi) {
    const char* ssid = wifi["ssid"] | "";
    const char* pass = wifi["password"] | "";
    strncpy(cfg.wifi.ssid, ssid, sizeof(cfg.wifi.ssid) - 1);
    strncpy(cfg.wifi.password, pass, sizeof(cfg.wifi.password) - 1);
    cfg.wifi.apModePreferred = wifi["apMode"] | true;
    cfg.wifi.apCheckInterval = wifi["apCheckInterval"] | 1000UL;
    cfg.wifi.heartbeatInterval = wifi["heartbeatInterval"] | 10000UL;
  }
  
  // --- Server Section ---
  JsonObject server = doc["server"];
  if (server) {
    const char* host = server["host"] | DEFAULT_TSW_HOST;
    strncpy(cfg.server.host, host, sizeof(cfg.server.host) - 1);
    cfg.server.port = server["port"] | DEFAULT_TSW_PORT;
    const char* apiKey = server["apiKey"] | "";
    strncpy(cfg.server.apiKey, apiKey, sizeof(cfg.server.apiKey) - 1);
  } else {
    // Backward compatibility: apiKey at root level
    const char* apiKey = doc["apiKey"] | "";
    strncpy(cfg.server.apiKey, apiKey, sizeof(cfg.server.apiKey) - 1);
  }
  
  // --- Device Section ---
  JsonObject device = doc["device"];
  if (device) {
    const char* name = device["name"] | DEFAULT_DEVICE_NAME;
    const char* mdns = device["mdnsName"] | DEFAULT_MDNS_NAME;
    strncpy(cfg.device.name, name, sizeof(cfg.device.name) - 1);
    strncpy(cfg.device.mdnsName, mdns, sizeof(cfg.device.mdnsName) - 1);
  }

  LOG_SYS_INFO("Config loaded:\n");
  LOG_SYS_INFO("  WiFi SSID: %s, AP-Mode: %s\n", 
               cfg.wifi.ssid, cfg.wifi.apModePreferred ? "true" : "false");
  LOG_SYS_INFO("  Server: %s:%d\n", cfg.server.host, cfg.server.port);
  LOG_SYS_INFO("  Device: %s (mDNS: %s)\n", cfg.device.name, cfg.device.mdnsName);
  
  return true;
}

// =============================================================================
// Save Configuration to JSON
// =============================================================================

inline void saveConfig() {
  LOG_SYS_INFO("Saving config to /config.json...\n");
  
  if (!LittleFS.begin()) {
    LittleFS.begin(true); // Format if needed
  }
  
  File f = LittleFS.open("/config.json", "w");
  if (!f) {
    LOG_SYS_ERROR("Failed to open config file for writing\n");
    return;
  }

  DynamicJsonDocument doc(2048);
  
  // WiFi section
  JsonObject wifi = doc.createNestedObject("wifi");
  wifi["ssid"] = cfg.wifi.ssid;
  wifi["password"] = cfg.wifi.password;
  wifi["apMode"] = cfg.wifi.apModePreferred;
  wifi["apCheckInterval"] = cfg.wifi.apCheckInterval;
  wifi["heartbeatInterval"] = cfg.wifi.heartbeatInterval;
  
  // Server section
  JsonObject server = doc.createNestedObject("server");
  server["host"] = cfg.server.host;
  server["port"] = cfg.server.port;
  server["apiKey"] = cfg.server.apiKey;
  
  // Device section
  JsonObject device = doc.createNestedObject("device");
  device["name"] = cfg.device.name;
  device["mdnsName"] = cfg.device.mdnsName;

  serializeJsonPretty(doc, f);
  f.close();
  
  LOG_SYS_INFO("Config saved successfully\n");
}

/**
 * Serialize current config to a JsonDocument for REST API.
 * @param doc  JsonDocument to populate
 */
inline void configToJson(JsonDocument& doc) {
  // WiFi section
  JsonObject wifi = doc.createNestedObject("wifi");
  wifi["ssid"] = cfg.wifi.ssid;
  wifi["password"] = "********";  // Don't expose password in API
  wifi["apMode"] = cfg.wifi.apModePreferred;
  wifi["apCheckInterval"] = cfg.wifi.apCheckInterval;
  wifi["heartbeatInterval"] = cfg.wifi.heartbeatInterval;
  
  // Server section
  JsonObject server = doc.createNestedObject("server");
  server["host"] = cfg.server.host;
  server["port"] = cfg.server.port;
  server["apiKey"] = cfg.server.apiKey;  // Don't expose API key
  
  // Device section
  JsonObject device = doc.createNestedObject("device");
  device["name"] = cfg.device.name;
  device["mdnsName"] = cfg.device.mdnsName;
  
  // Build info
  JsonObject build = doc.createNestedObject("build");
  build["number"] = BUILD_NUMBER;
  build["date"] = BUILD_DATE;
}
