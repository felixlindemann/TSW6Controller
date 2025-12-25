#pragma once
#include <Arduino.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "config.h"

struct Config
{
  char ssid[32];
  char pass[64];
  char apiKey[64];
  bool apModePreferred; // true = Access Point, false = Client (STA)
  unsigned long wifiApCheckIntervall;
  unsigned long wifiHeartbeatIntervall;
};

extern Config cfg;

inline bool loadConfig()
{
  if (!LittleFS.begin())
    return false;
  File f = LittleFS.open("/config.json", "r");
  if (!f)
    return false;

  DynamicJsonDocument doc(1024);
  DeserializationError err = deserializeJson(doc, f);
  f.close();

  if (err) {
    TRACE_PRINT("[Config] JSON parse error: %s\n", err.c_str());
    return false;
  }

  // Defaults setzen
  memset(&cfg, 0, sizeof(cfg));
  cfg.apModePreferred = true;

  // Werte aus JSON lesen
  JsonObject wifi = doc["wifi"];
  String ssid = wifi["ssid"] | "";
  String pass = wifi["password"] | "";
  String apiKey = doc["apiKey"] | "";
  
  ssid.toCharArray(cfg.ssid, sizeof(cfg.ssid));
  pass.toCharArray(cfg.pass, sizeof(cfg.pass));
  apiKey.toCharArray(cfg.apiKey, sizeof(cfg.apiKey));
  
  cfg.apModePreferred = wifi["apMode"] | true;
  cfg.wifiApCheckIntervall = wifi["wifiApCheckIntervall"] | 1000UL;
  cfg.wifiHeartbeatIntervall = wifi["wifiHeartbeatIntervall"] | 10000UL;

  return true;
}

inline void saveConfig()
{
  if (!LittleFS.begin())
    LittleFS.begin();
  File f = LittleFS.open("/config.json", "w");
  if (!f)
    return;

  DynamicJsonDocument doc(1024);
  JsonObject wifi = doc.createNestedObject("wifi");
  wifi["ssid"] = cfg.ssid;
  wifi["password"] = cfg.pass;
  wifi["apMode"] = cfg.apModePreferred;
  wifi["wifiApCheckIntervall"] = cfg.wifiApCheckIntervall;
  wifi["wifiHeartbeatIntervall"] = cfg.wifiHeartbeatIntervall;
  
  doc["apiKey"] = cfg.apiKey;

  serializeJsonPretty(doc, f);
  f.close();
}
