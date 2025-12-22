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
TSWLever::TSWLever(uint8_t pin, const String &ctrl, TSWSpider *s)
    : AnalogSlider(ctrl + "_HW", pin),
      TSWControl(ctrl, s),
      lastSentValue(-999.0f) {}

// --- Load Notch configuration ---
void TSWLever::loadNotches(const String &filePath)
{
  notches.loadFromFile(filePath);
}

void TSWLever::setControllerName(const String &controller)
{
  controllerName = controller;
}
void TSWLever::setinverted(bool inv)
{
  AnalogSlider::setInverted(inv);
}
// --- Update and send value ---
void TSWLever::updateAndSend()
{
  if (update())
  {
    int percent = getPercentValue(); // 0–100 %
    int raw = getCurrentRawValue();
    int lastRaw = getLastRawValue();

    if (abs(raw - lastRaw) > 5)
    {
      // Significant raw change detected
      lastChangeReason = "raw change";

      float tswValue = notches.hasPositions()
                           ? notches.mapToTSW(percent)
                           : percent / 100.0f;

      spider->setControllerValue(controllerName, tswValue);
      lastSentValue = tswValue;

#if TRACE && TRACE_HARDWARE
      Serial.printf("[TSWLever] %s: raw=%d pct=%d -> TSW=%.3f\n",
                    controllerName.c_str(), raw, percent, tswValue);
#endif
    }
    else
    {
      // lastChangeReason = "stable";
    }
  }
}
