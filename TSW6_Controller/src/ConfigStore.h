#pragma once
#include <Arduino.h>
#include <LittleFS.h>
#include "config.h"


struct Config {
  char ssid[32];
  char pass[64];
  char apiKey[64];
  bool apModePreferred;  // true = Access Point, false = Client (STA)
};

Config cfg;

// Hilfsfunktion: String aus Datei lesen und sicher in char[] kopieren
static void readLineToBuf(File &f, char* buf, size_t buflen) {
  String s = f.readStringUntil('\n');
  s.trim();
  s.toCharArray(buf, buflen);
}

bool loadConfig() {
  if (!LittleFS.begin()) return false;
  File f = LittleFS.open("/wifi.cfg", "r");
  if (!f) return false;

  // Defaults vorlesen
  memset(&cfg, 0, sizeof(cfg));
  cfg.apModePreferred = true; // Default: AP, falls Datei kürzer ist

  readLineToBuf(f, cfg.ssid, sizeof(cfg.ssid));
  readLineToBuf(f, cfg.pass, sizeof(cfg.pass));
  readLineToBuf(f, cfg.apiKey, sizeof(cfg.apiKey));

  // 4. Zeile: Modus (1 = AP, 0 = STA). Fehlende Zeile => Default bleibt true.
  if (f.available()) {
    String mode = f.readStringUntil('\n');
    mode.trim();
    if (mode.length()) cfg.apModePreferred = (mode.toInt() == 1);
  }
  f.close();
  return true;
}

void saveConfig() {
  if (!LittleFS.begin()) LittleFS.begin();
  File f = LittleFS.open("/wifi.cfg", "w");
  if (!f) return;
  f.println(cfg.ssid);
  f.println(cfg.pass);
  f.println(cfg.apiKey);
  f.println(cfg.apModePreferred ? "1" : "0");
  f.close();
}
