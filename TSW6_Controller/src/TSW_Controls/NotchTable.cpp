/**
 * @file NotchTable.cpp
 * @brief Implementation of the NotchTable class for mapping analog inputs to TSW values.
 *
 * @author Felix Lindemann
 * @date 2025-10-26
 * @version 1.0
 */

#include "NotchTable.h"

bool NotchTable::loadFromFile(const String &path) {
  File file = SD.open(path);
  if (!file) {
    Serial.println("[NotchTable] Failed to open file: " + path);
    return false;
  }

  StaticJsonDocument<1024> doc;
  DeserializationError err = deserializeJson(doc, file);
  file.close();

  if (err) {
    Serial.println("[NotchTable] JSON parse error");
    return false;
  }

  controller = doc["controller"].as<String>();
  positions.clear();

  for (JsonObject n : doc["positions"].as<JsonArray>()) {
    Notch notch;
    notch.label = n["label"].as<String>();
    notch.tswValue = n["tsw"].as<float>();
    notch.rangeMin = n["range"][0].as<int>();
    notch.rangeMax = n["range"][1].as<int>();
    positions.push_back(notch);
  }

  return true;
}
bool NotchTable::loadFromArray(const std::vector<Notch>& list) {
  positions = list;
  controller = "Custom";
  return true;
}


float NotchTable::mapToTSW(int percent) const {
  for (const auto &n : positions) {
    if (percent >= n.rangeMin && percent <= n.rangeMax)
      return n.tswValue;
  }
  return 0.0f;
}
