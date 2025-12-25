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
  if (notches.loadFromFile(filePath)) {
    setControllerName(notches.getControllerName());
    LOG_SW_INFO("%s: Loaded %d notches for controller: %s\n", 
                getId().c_str(), notches.getPositionCount(), controllerName.c_str());
  } else {
    LOG_SW_ERROR("%s: Failed to load notches from: %s\n", getId().c_str(), filePath.c_str());
  }
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
  LOG_HW_TRACE("TSWLever Update: %s\n", getId().c_str());
  if (update())
  {
    int percent = getPercentValue(); // 0–100 %
    int raw = getCurrentRawValue();
    int lastRaw = getLastRawValue();

    if (abs(raw - lastRaw) > 5)
    {
      // Significant raw change detected
      lastChangeReason = "raw change";
      LOG_HW_DEBUG("%s (%s): raw change: last=%d current=%d --> %d%%\n",
              getId().c_str(), controllerName.c_str(), lastRaw, raw, percent);

      float tswValue = notches.hasPositions()
                           ? notches.mapToTSW(percent)
                           : percent / 100.0f;

      spider->setControllerValue(controllerName, tswValue);
      lastSentValue = tswValue;
      LOG_HW_TRACE("%s: raw=%d pct=%d -> TSW=%.3f\n",
                    controllerName.c_str(), raw, percent, tswValue);
    }
    else
    {
      // lastChangeReason = "stable";
    }
  }
}
