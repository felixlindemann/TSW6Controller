/**
 * @file TSWLever.cpp
 * @brief Implementation of the TSWLever class for TSW analog mapping.
 *
 * @details
 * Uses AnalogSlider hardware input, applies NotchTable mapping,
 * and sends corresponding values to TSWSpider.
 *
 * @author
 *   Felix Lindemann
 * @date
 *   2025-10-28
 * @version
 *   2.0
 */

#include "TSWLever.h"
#include <cmath>

// --- Constructor ---
TSWLever::TSWLever(uint8_t pin, const String& ctrl, TSWSpider* s)
    : AnalogSlider(ctrl + "_HW", pin),
      TSWControl(ctrl, s),
      lastSentValue(-999.0f) {}

// --- Load Notch configuration ---
void TSWLever::loadNotches(const String& filePath) {
  notches.loadFromFile(filePath);
}

// --- Update and send value ---
void TSWLever::updateAndSend() {
  if (update()) {
    int percent = getPercentValue();  // 0–100 %
    float tswValue = notches.hasPositions()
                         ? notches.mapToTSW(percent)
                         : percent / 100.0f;

    if (fabs(tswValue - lastSentValue) > 0.001f) {
      spider->setControllerValue(controllerName, tswValue);
      lastSentValue = tswValue;
    }
  }
}
