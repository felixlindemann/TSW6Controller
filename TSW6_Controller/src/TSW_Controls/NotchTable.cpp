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
  LOG_SW_DEBUG("Loading notch table: %s\n", path.c_str());
  
  File file = LittleFS.open(path);
  if (!file) {
    LOG_SW_ERROR("Failed to open notch file: %s\n", path.c_str());
    return false;
  }

  DynamicJsonDocument doc(8192);
  DeserializationError err = deserializeJson(doc, file);
  file.close();

  if (err) {
    LOG_SW_ERROR("JSON parse error in %s: %s\n", path.c_str(), err.c_str());
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

  LOG_SW_INFO("Loaded %d notches for controller '%s' from %s\n", 
              positions.size(), controller.c_str(), path.c_str());
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

void NotchTable::setupBinaryButton(const String &controllerName, 
                                   float releasedValue, 
                                   float pressedValue) {
  controller = controllerName;
  positions.clear();
  
  // Released state: 0% (button not pressed)
  Notch released;
  released.label = "Released";
  released.tswValue = releasedValue;
  released.rangeMin = 0;
  released.rangeMax = 49;
  positions.push_back(released);
  
  // Pressed state: 100% (button pressed)
  Notch pressed;
  pressed.label = "Pressed";
  pressed.tswValue = pressedValue;
  pressed.rangeMin = 50;
  pressed.rangeMax = 100;
  positions.push_back(pressed);
}
